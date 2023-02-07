/*
 * ServerSocket.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */

#include "Communication/Ethernet/TCP/ServerSocket.hpp"
#ifdef HAL_ETH_MODULE_ENABLED

uint8_t ServerSocket::priority = 0;
unordered_map<uint32_t,ServerSocket*> ServerSocket::listening_sockets = {};

ServerSocket::ServerSocket() = default;

ServerSocket::ServerSocket(IPV4 local_ip, uint32_t local_port) : local_ip(local_ip),local_port(local_port){
	state = INACTIVE;
	server_control_block = tcp_new();
	tcp_nagle_disable(server_control_block);
	err_t error = tcp_bind(server_control_block, &local_ip.address, local_port);

	if(error == ERR_OK){
		server_control_block = tcp_listen(server_control_block);
		state = LISTENING;
		listening_sockets[local_port] = this;
		tcp_accept(server_control_block, accept_callback);
	}else{
		memp_free(MEMP_TCP_PCB, server_control_block);
		//TODO: Error Handler
	}
}

ServerSocket::ServerSocket(string local_ip, uint32_t local_port) : ServerSocket(IPV4(local_ip),local_port){}

ServerSocket::ServerSocket(EthernetNode local_node) : ServerSocket(local_node.ip,local_node.port){};

ServerSocket::~ServerSocket(){
	close();
}

void ServerSocket::close(){
	// Clean all callbacks
	tcp_arg(client_control_block, nullptr);
	tcp_sent(client_control_block, nullptr);
	tcp_recv(client_control_block, nullptr);
	tcp_err(client_control_block, nullptr);
	tcp_poll(client_control_block, nullptr, 0);

	tcp_close(client_control_block);
	while(pbuf_free(tx_packet_buffer) != 0);

	memp_free(MEMP_TCP_PCB, client_control_block);

	listening_sockets[local_port] = this;
	state = LISTENING;

	priority--;

}

void ServerSocket::process_data(){
	uint8_t* new_data = (uint8_t*)rx_packet_buffer->payload;
	uint16_t id = Packet<>::get_id(new_data);
	if(Packet<>::save_by_id.contains(id)){
		Packet<>::save_by_id[id](new_data);
	}

	if(Packet<>::on_received.contains(id)){
		Packet<>::on_received[id]();
	}
	tcp_recved(client_control_block, rx_packet_buffer->tot_len);
	pbuf_free(rx_packet_buffer);
	rx_packet_buffer = rx_packet_buffer->next;
}

void ServerSocket::send(){
	pbuf* temporal_packet_buffer;
	err_t error = ERR_OK;
	while(error == ERR_OK && tx_packet_buffer != nullptr && tx_packet_buffer->len <= tcp_sndbuf(client_control_block)){
		temporal_packet_buffer = tx_packet_buffer;
		error = tcp_write(client_control_block, tx_packet_buffer->payload, tx_packet_buffer->len, 0);
		if(error == ERR_OK){
			tcp_output(client_control_block);
			tx_packet_buffer = tx_packet_buffer->next;
			if(tx_packet_buffer != nullptr){
				pbuf_ref(tx_packet_buffer);
			}
			memp_free_pool(memp_pools[PBUF_POOL_MEMORY_DESC_POSITION],temporal_packet_buffer);
		}else if(error == ERR_MEM){			//Low on memory
			tx_packet_buffer = temporal_packet_buffer;
		}else{
			//TODO: Error Handler
		}
	}
}

err_t ServerSocket::accept_callback(void* arg, struct tcp_pcb* incomming_control_block, err_t error){
	if(listening_sockets.contains(incomming_control_block->local_port)){
		ServerSocket* server_socket = listening_sockets[incomming_control_block->local_port];
		listening_sockets.erase(incomming_control_block->local_port);

		server_socket->state = ACCEPTED;
		server_socket->client_control_block = incomming_control_block;
		server_socket->tx_packet_buffer = nullptr;
	
		tcp_setprio(incomming_control_block, priority);
		tcp_nagle_disable(incomming_control_block);
		tcp_arg(incomming_control_block, server_socket);
		tcp_recv(incomming_control_block, receive_callback);
		tcp_sent(incomming_control_block, send_callback);
		tcp_err(incomming_control_block, error_callback);
		tcp_poll(incomming_control_block, poll_callback , 0);

		priority++;

		return ERR_OK;
	}else
		return ERROR;
}

err_t ServerSocket::receive_callback(void* arg, struct tcp_pcb* client_control_block, struct pbuf* packet_buffer, err_t error){

	ServerSocket* server_socket = (ServerSocket*) arg;
	server_socket->client_control_block = client_control_block;

	if(packet_buffer == nullptr){									//If RECEIVING buffer is empty, is a signal to close connection
		server_socket->state = CLOSING;
		pbuf_free(server_socket->tx_packet_buffer);
		server_socket->close();
		return ERR_OK;
	}
	else if(error != ERR_OK){										//Check if packet is valid
		if(packet_buffer != nullptr){
			pbuf_free(packet_buffer);
		}
		return error;
	}
	else if(server_socket->state == ACCEPTED){
		if(server_socket->rx_packet_buffer == nullptr){
			server_socket->rx_packet_buffer = packet_buffer;
			server_socket->process_data();
		}else{
			pbuf_chain(server_socket->rx_packet_buffer, packet_buffer);
		}
		return ERR_OK;
	}
	else if(server_socket->state == CLOSING){
		pbuf_free(server_socket->rx_packet_buffer);
		server_socket->rx_packet_buffer = nullptr;
		pbuf_free(packet_buffer);
		return ERR_OK;
	}
	return ERR_OK;
}

void ServerSocket::error_callback(void *arg, err_t error){
	ServerSocket* server_socket = (ServerSocket*) arg;
	if(error == ERR_RST || error == ERR_ABRT){
		server_socket->close();
	}
	//TODO: Error Handler
}

err_t ServerSocket::poll_callback(void *arg, struct tcp_pcb *client_control_block){
	ServerSocket* server_socket = (ServerSocket*)arg;
	server_socket->client_control_block = client_control_block;

	if(server_socket != nullptr){
		if(server_socket->tx_packet_buffer != nullptr){
			server_socket->send();
		}else{
			if(server_socket->state == CLOSING){
				server_socket->close();
			}
		}
		return ERR_OK;
	}
	else{
		tcp_abort(client_control_block);
		return ERR_ABRT;
	}

}

err_t ServerSocket::send_callback(void *arg, struct tcp_pcb *client_control_block, u16_t len){
	ServerSocket* server_socket = (ServerSocket*)arg;
	server_socket->client_control_block = client_control_block;
	if(server_socket->tx_packet_buffer != nullptr){
		tcp_sent(client_control_block, send_callback);
		server_socket->send();
	}
	else{
		if(server_socket->state == CLOSING){
			server_socket->close();
		}
	}
	return ERR_OK;
}


#endif
