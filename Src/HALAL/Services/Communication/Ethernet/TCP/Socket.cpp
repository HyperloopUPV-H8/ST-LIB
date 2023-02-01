/*
 * Socket.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */

#include "Communication/Ethernet/TCP/Socket.hpp"
#ifdef HAL_ETH_MODULE_ENABLED

unordered_map<EthernetNode,Socket*> Socket::connecting_sockets = {};

Socket::Socket() = default;

Socket::Socket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port):remote_ip(remote_ip), remote_port(remote_port){
	state = INACTIVE;
	EthernetNode remote_node(remote_ip, remote_port);

	connection_control_block = tcp_new();
	tcp_bind(connection_control_block, &local_ip.address, local_port);
	tcp_nagle_disable(connection_control_block);

	connecting_sockets[remote_node] = this;
	tcp_connect(connection_control_block, &remote_ip.address , remote_port, connect_callback);
}

Socket::Socket(string local_ip, uint32_t local_port, string remote_ip, uint32_t remote_port):Socket(IPV4(local_ip),local_port,IPV4(remote_ip),remote_port){}


Socket::Socket(EthernetNode local_node, EthernetNode remote_node):Socket(local_node.ip, local_node.port, remote_node.ip, remote_node.port){}

void Socket::close(){
  tcp_arg(socket_control_block, nullptr);
  tcp_sent(socket_control_block, nullptr);
  tcp_recv(socket_control_block, nullptr);
  tcp_err(socket_control_block, nullptr);
  tcp_poll(socket_control_block, nullptr, 0);

  if(tx_packet_buffer!=nullptr){
	  pbuf_free(tx_packet_buffer);
  }
  if(rx_packet_buffer!=nullptr){
	  pbuf_free(rx_packet_buffer);
  }

  tcp_close(socket_control_block);
}

void Socket::reconnect(){
	EthernetNode remote_node(remote_ip, remote_port);
	if(!connecting_sockets.contains(remote_node)){
		connecting_sockets[remote_node] = this;
	}
	tcp_connect(connection_control_block, &remote_ip.address , remote_port, connect_callback);
}


void Socket::send(){
	uint16_t is_freed;
	pbuf* temporal_packet_buffer;
	err_t error = ERR_OK;
	while(error == ERR_OK && tx_packet_buffer != nullptr && tx_packet_buffer->len <= tcp_sndbuf(socket_control_block)){
		temporal_packet_buffer = tx_packet_buffer;
		error = tcp_write(socket_control_block, tx_packet_buffer->payload, tx_packet_buffer->len, 0);
		if(error == ERR_OK){
			tcp_output(socket_control_block);
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

void Socket::process_data(){
	uint8_t* new_data = (uint8_t*)rx_packet_buffer->payload;
	uint16_t id = Packet<>::get_id(new_data);
	if(Packet<>::save_by_id.contains(id)){
		Packet<>::save_by_id[id](new_data);
	}
	
	if(Packet<>::on_received.contains(id)){
		Packet<>::on_received[id]();
	}
}

err_t Socket::connect_callback(void* arg, struct tcp_pcb* client_control_block, err_t error){
	IPV4 remote_ip;
	remote_ip.address = client_control_block->remote_ip;
	EthernetNode remote_node(remote_ip,client_control_block->remote_port);

	if(connecting_sockets.contains(remote_node)){
		Socket* socket = connecting_sockets[remote_node];
		connecting_sockets.erase(remote_node);

		socket->socket_control_block = client_control_block;
		socket->state = CONNECTED;
		
		tcp_nagle_disable(client_control_block);
		tcp_arg(client_control_block, socket);
		tcp_recv(client_control_block, receive_callback);
		tcp_poll(client_control_block, poll_callback,0);
		tcp_sent(client_control_block, send_callback);
		tcp_err(client_control_block, error_callback);
		return ERR_OK;
	}else return ERROR;
}

err_t Socket::receive_callback(void* arg, struct tcp_pcb* client_control_block, struct pbuf* packet_buffer, err_t error){
	Socket* socket = (Socket*)arg;
	socket->socket_control_block = client_control_block;
	if(packet_buffer == nullptr){ 		//If empty packet is received, connection is closed
		socket->state = CLOSING;
		if(socket->tx_packet_buffer == nullptr){   //Check if there is data waiting to be sent
			socket->close();
		}else{
			tcp_sent(client_control_block, send_callback);
			socket->send();
		}
		return ERR_OK;
	}else if(error != ERR_OK){
		socket->tx_packet_buffer = nullptr;
		pbuf_free(packet_buffer);
		return error;
	}else if(socket->state == CONNECTED){
		socket->rx_packet_buffer = packet_buffer;
		tcp_recved(client_control_block, packet_buffer->tot_len);
		socket->process_data();
		pbuf_free(packet_buffer);
		return ERR_OK;
	}else{
		tcp_recved(client_control_block, packet_buffer->tot_len);
		socket->tx_packet_buffer = nullptr;
		pbuf_free(packet_buffer);
		return ERR_OK;
	}
}

err_t Socket::poll_callback(void* arg, struct tcp_pcb* client_control_block){
	Socket* socket = (Socket*)arg;
	socket->socket_control_block = client_control_block;
	if(socket != nullptr){
		if(socket->tx_packet_buffer!=nullptr){
			socket->send();
		}
		if(socket->state == CLOSING){
			socket->close();
		}
		return ERR_OK;
	}else{
		tcp_abort(client_control_block);
		return ERR_ABRT;
	}
}

err_t Socket::send_callback(void* arg, struct tcp_pcb* client_control_block, uint16_t length){
	Socket* socket = (Socket*)arg;
	socket->socket_control_block = client_control_block;
	if(socket->tx_packet_buffer != nullptr){
		socket->send();
	}else if(socket->state == CLOSING){
		socket->close();
	}
	return ERR_OK;
}

void Socket::error_callback(void *arg, err_t error){
	Socket* socket = (Socket*) arg;
	if(error == ERR_RST || error == ERR_ABRT){
		socket->close();
	}
	//TODO: Error Handler
}

#endif

