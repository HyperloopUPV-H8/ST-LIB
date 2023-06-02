/*
 * ServerSocket.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */

#include "Communication/Ethernet/TCP/ServerSocket.hpp"
#include "lwip/priv/tcp_priv.h"
#include "ErrorHandler/ErrorHandler.hpp"
#ifdef HAL_ETH_MODULE_ENABLED

uint8_t ServerSocket::priority = 1;
unordered_map<uint32_t,ServerSocket*> ServerSocket::listening_sockets = {};

ServerSocket::ServerSocket() = default;

ServerSocket::ServerSocket(IPV4 local_ip, uint32_t local_port) : local_ip(local_ip),local_port(local_port){
	if(not Ethernet::is_running) {
		ErrorHandler("Cannot declare UDP socket before Ethernet::start()");
		return;
	}
	tx_packet_buffer = {};
	rx_packet_buffer = {};
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
		ErrorHandler("Cannnot bind server socket, memory low");
	}
	OrderProtocol::sockets.push_back(this);
}

ServerSocket::ServerSocket(ServerSocket&& other) : server_control_block(move(other.server_control_block)), local_ip(move(other.local_ip)), local_port(move(other.local_port))
, state(other.state){
	listening_sockets[local_port] = this;
	tx_packet_buffer = {};
	rx_packet_buffer = {};
}

void ServerSocket::operator=(ServerSocket&& other){
	local_ip = move(other.local_ip);
	local_port = move(other.local_port);
	server_control_block = move(other.server_control_block);
	state = other.state;
	listening_sockets[local_port] = this;
	tx_packet_buffer = {};
	rx_packet_buffer = {};
}

ServerSocket::~ServerSocket(){
	OrderProtocol::sockets.erase(std::remove(OrderProtocol::sockets.begin(), OrderProtocol::sockets.end(), this), OrderProtocol::sockets.end());
}

ServerSocket::ServerSocket(string local_ip, uint32_t local_port) : ServerSocket(IPV4(local_ip),local_port){}

ServerSocket::ServerSocket(EthernetNode local_node) : ServerSocket(local_node.ip,local_node.port){};

void ServerSocket::close(){
	// Clean all callbacks
	tcp_arg(client_control_block, nullptr);
	tcp_sent(client_control_block, nullptr);
	tcp_recv(client_control_block, nullptr);
	tcp_err(client_control_block, nullptr);
	tcp_poll(client_control_block, nullptr, 0);

	tcp_close(client_control_block);
	while(!tx_packet_buffer.empty()){
		pbuf_free(tx_packet_buffer.front());
		tx_packet_buffer.pop();
	}
	while(!rx_packet_buffer.empty()){
		pbuf_free(rx_packet_buffer.front());
		rx_packet_buffer.pop();
	}

    tcp_pcb_remove(&tcp_active_pcbs, client_control_block);
    tcp_free(client_control_block);

	listening_sockets[local_port] = this;
	state = LISTENING;

	priority--;

}

void ServerSocket::process_data(){
	while(!rx_packet_buffer.empty()){
		struct pbuf* packet = rx_packet_buffer.front();
		rx_packet_buffer.pop();
		uint8_t* new_data = (uint8_t*)(packet->payload);
		Order::process_data(this, new_data);
		tcp_recved(client_control_block, packet->tot_len);
		pbuf_free(packet);
	}
}

void ServerSocket::send(){
	pbuf* temporal_packet_buffer;
	err_t error = ERR_OK;
	while(error == ERR_OK && !tx_packet_buffer.empty() && tx_packet_buffer.front()->len <= tcp_sndbuf(client_control_block)){
		temporal_packet_buffer = tx_packet_buffer.front();
		error = tcp_write(client_control_block, temporal_packet_buffer->payload, temporal_packet_buffer->len, TCP_WRITE_FLAG_COPY);
		if(error == ERR_OK){
			tx_packet_buffer.pop();
			tcp_output(client_control_block);
			memp_free_pool(memp_pools[PBUF_POOL_MEMORY_DESC_POSITION],temporal_packet_buffer);
		}else{
			ErrorHandler("Cannot write to socket, error: %d", error);
		}
	}
}

err_t ServerSocket::accept_callback(void* arg, struct tcp_pcb* incomming_control_block, err_t error){
	if(listening_sockets.contains(incomming_control_block->local_port)){
		ServerSocket* server_socket = listening_sockets[incomming_control_block->local_port];
		listening_sockets.erase(incomming_control_block->local_port);

		server_socket->state = ACCEPTED;
		server_socket->client_control_block = incomming_control_block;
		server_socket->tx_packet_buffer = {};
		server_socket->rx_packet_buffer = {};

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

	if(packet_buffer == nullptr){      //FIN has been received
		server_socket->state = CLOSING;
		return ERR_OK;
	}

	if(error != ERR_OK){										//Check if packet is valid
		if(packet_buffer != nullptr){
			pbuf_free(packet_buffer);
		}
		return error;
	}
	else if(server_socket->state == ACCEPTED){
		server_socket->rx_packet_buffer.push(packet_buffer);
		server_socket->process_data();
		return ERR_OK;
	}
	else if(server_socket->state == CLOSING){ 		//Socket is already closed
		while(not server_socket->rx_packet_buffer.empty()){
			pbuf_free(server_socket->rx_packet_buffer.front());
			server_socket->rx_packet_buffer.pop();
		}
		server_socket->rx_packet_buffer = {};
		pbuf_free(packet_buffer);
		return ERR_OK;
	}
	return ERR_OK;
}

void ServerSocket::error_callback(void *arg, err_t error){
	ServerSocket* server_socket = (ServerSocket*) arg;
	server_socket->close();
	ErrorHandler("Socket error: %d. Socket closed", error);
}

err_t ServerSocket::poll_callback(void *arg, struct tcp_pcb *client_control_block){
	ServerSocket* server_socket = (ServerSocket*)arg;
	server_socket->client_control_block = client_control_block;

	if(server_socket == nullptr){    //Polling non existing pcb, fatal error
		tcp_abort(client_control_block);
		return ERR_ABRT;
	}

	while(not server_socket->tx_packet_buffer.empty()){		//TX FIFO is not empty
		server_socket->send();
	}

	while(not server_socket->rx_packet_buffer.empty()){		//RX FIFO is not empty
		server_socket->process_data();
	}

	if(server_socket->state == CLOSING){ //pcb has been polled to close
		server_socket->close();
	}

	return ERR_OK;
}

err_t ServerSocket::send_callback(void *arg, struct tcp_pcb *client_control_block, u16_t len){
	ServerSocket* server_socket = (ServerSocket*)arg;
	server_socket->client_control_block = client_control_block;
	if(!server_socket->tx_packet_buffer.empty()){
		server_socket->send();
	}
	else if(server_socket->state == CLOSING){
		server_socket->close();
	}
	return ERR_OK;
}


#endif
