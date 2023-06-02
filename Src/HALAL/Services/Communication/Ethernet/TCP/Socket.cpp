/*
 * Socket.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */
#include "Communication/Ethernet/TCP/Socket.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#ifdef HAL_ETH_MODULE_ENABLED

unordered_map<EthernetNode,Socket*> Socket::connecting_sockets = {};

Socket::Socket() = default;

Socket::Socket(Socket&& other):remote_port(move(remote_port)), connection_control_block(move(other.connection_control_block)),
	 state(other.state){
	EthernetNode remote_node(other.remote_ip, other.remote_port);
	connecting_sockets[remote_node] = this;
}

void Socket::operator=(Socket&& other){
	connection_control_block = move(other.connection_control_block);
	remote_port = move(remote_port);
	state = other.state;
	EthernetNode remote_node(other.remote_ip, other.remote_port);
	connecting_sockets[remote_node] = this;
}

Socket::~Socket(){
	OrderProtocol::sockets.erase(std::remove(OrderProtocol::sockets.begin(), OrderProtocol::sockets.end(), this), OrderProtocol::sockets.end());
}

Socket::Socket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port):remote_ip(remote_ip), remote_port(remote_port){
	if(not Ethernet::is_running) {
		ErrorHandler("Cannot declare TCP socket before Ethernet::start()");
		return;
	}
	state = INACTIVE;
	EthernetNode remote_node(remote_ip, remote_port);

	connection_control_block = tcp_new();
	tcp_bind(connection_control_block, &local_ip.address, local_port);
	tcp_nagle_disable(connection_control_block);

	connecting_sockets[remote_node] = this;
	tcp_connect(connection_control_block, &remote_ip.address , remote_port, connect_callback);
	OrderProtocol::sockets.push_back(this);
}

Socket::Socket(string local_ip, uint32_t local_port, string remote_ip, uint32_t remote_port):Socket(IPV4(local_ip),local_port,IPV4(remote_ip),remote_port){}


Socket::Socket(EthernetNode local_node, EthernetNode remote_node):Socket(local_node.ip, local_node.port, remote_node.ip, remote_node.port){}

void Socket::close(){
  tcp_arg(socket_control_block, nullptr);
  tcp_sent(socket_control_block, nullptr);
  tcp_recv(socket_control_block, nullptr);
  tcp_err(socket_control_block, nullptr);
  tcp_poll(socket_control_block, nullptr, 0);

	while(!tx_packet_buffer.empty()){
		pbuf_free(tx_packet_buffer.front());
		tx_packet_buffer.pop();
	}
	while(!rx_packet_buffer.empty()){
		pbuf_free(rx_packet_buffer.front());
		rx_packet_buffer.pop();
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
	pbuf* temporal_packet_buffer;
	err_t error = ERR_OK;
	while(error == ERR_OK && !tx_packet_buffer.empty() && tx_packet_buffer.front()->len <= tcp_sndbuf(socket_control_block)){
		temporal_packet_buffer = tx_packet_buffer.front();
		error = tcp_write(socket_control_block, temporal_packet_buffer->payload, temporal_packet_buffer->len, TCP_WRITE_FLAG_COPY);
		if(error == ERR_OK){
			tx_packet_buffer.pop();
			tcp_output(socket_control_block);
			memp_free_pool(memp_pools[PBUF_POOL_MEMORY_DESC_POSITION],temporal_packet_buffer);
		}else{
			ErrorHandler("Cannot write to client socket. Error code: %d",error);
		}
	}
}

void Socket::process_data(){
	while(!rx_packet_buffer.empty()){
		struct pbuf* packet = rx_packet_buffer.front();
		rx_packet_buffer.pop();
		uint8_t* new_data = (uint8_t*)(packet->payload);
		Order::process_data(this, new_data);
		tcp_recved(socket_control_block, packet->tot_len);
		pbuf_free(packet);
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
	if(packet_buffer == nullptr){ 		//FIN is received
		socket->state = CLOSING;
		return ERR_OK;
	}
	if(error != ERR_OK){
		if(packet_buffer != nullptr){
			pbuf_free(packet_buffer);
		}
		return error;
	}else if(socket->state == CONNECTED){
		socket->rx_packet_buffer.push(packet_buffer);
		tcp_recved(client_control_block, packet_buffer->tot_len);
		socket->process_data();
		pbuf_free(packet_buffer);
		return ERR_OK;
	}else{
		tcp_recved(client_control_block, packet_buffer->tot_len);
		socket->tx_packet_buffer = {};
		pbuf_free(packet_buffer);
		return ERR_OK;
	}
}

err_t Socket::poll_callback(void* arg, struct tcp_pcb* client_control_block){
	Socket* socket = (Socket*)arg;
	socket->socket_control_block = client_control_block;
	if(socket != nullptr){
		while(not socket->tx_packet_buffer.empty()){
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
	if(not socket->tx_packet_buffer.empty()){
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
	ErrorHandler("Client socket error: %d. Socket closed",error);
}

#endif

