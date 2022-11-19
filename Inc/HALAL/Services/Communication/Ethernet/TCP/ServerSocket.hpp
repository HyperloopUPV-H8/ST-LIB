/*
 * ServerSocket.hpp
 *
 *  Created on: 14 nov. 2022
 *      Author: stefa
 */
#pragma once

#include "Communication/Ethernet/EthernetNode.hpp"
#include "Packets/Packet.hpp"
#include "Packets/Order.hpp"

class ServerSocket{
public:

	enum ServerState{
		INACTIVE,
		LISTENING,
		ACCEPTED,
		RECEIVED,
		CLOSING
	};

	static unordered_map<uint32_t,ServerSocket*> listening_sockets;
	struct tcp_pcb* server_control_block;
	struct pbuf* server_packet_bufer;
	IPV4 local_ip;
	uint32_t local_port;
	ServerState state;


	ServerSocket() = default;

	ServerSocket(IPV4 local_ip, uint32_t local_port) : local_ip(local_ip),local_port(local_port){

		state = INACTIVE;

		server_control_block = tcp_new();

		tcp_nagle_disable(server_control_block);

		err_t error = tcp_bind(server_control_block, &local_ip.ip_address, local_port);

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

	ServerSocket(string local_ip, uint32_t local_port) : ServerSocket(IPV4(local_ip),local_port){}

	ServerSocket(EthernetNode local_node);

	~ServerSocket(){
		close();
	}

	void close(){
	  // Clean all callbacks
	  tcp_arg(client_control_block, nullptr);
	  tcp_sent(client_control_block, nullptr);
	  tcp_recv(client_control_block, nullptr);
	  tcp_err(client_control_block, nullptr);
	  tcp_poll(client_control_block, nullptr, 0);

	  tcp_close(this->client_control_block);
	  while(server_packet_bufer)
	  memp_free(MEMP_TCP_PCB, client_control_block);

	  listening_sockets[local_port] = this;
	  state = LISTENING;

	}

	void process_data(){

		char buf[100];
		memset (buf, '\0', 100);

		const char* message = "RECEIVED";
		memcpy(buf,message,strlen(message));

		server_packet_bufer->payload = (void *)buf;
		server_packet_bufer->tot_len = strlen(message);
		server_packet_bufer->len = strlen(message);

		send();

	}

	template<class Type, class...Types>
	void send_order(Order<Type,Types...>& order){
		if(state != ACCEPTED && state != RECEIVED){
			return;
		}
		if(state == ACCEPTED){
			state = RECEIVED;
			tcp_sent(client_control_block, send_callback);
		}

		order.build();

		if(order.bffr_size > tcp_sndbuf(client_control_block)){
			tcp_pcb_purge(client_control_block);
			return;
		}

		server_packet_bufer = pbuf_alloc(PBUF_TRANSPORT, order.bffr_size, PBUF_POOL);

		pbuf_take(server_packet_bufer, order.bffr, order.bffr_size);

		send();
	}

	void send(){
		err_t error = ERR_OK;
		while(error == ERR_OK && server_packet_bufer != nullptr && server_packet_bufer->tot_len <= tcp_sndbuf(client_control_block)){
			error = tcp_write(client_control_block, server_packet_bufer->payload, server_packet_bufer->tot_len, TCP_WRITE_FLAG_COPY);
			if(error == ERR_OK){
				tcp_output(client_control_block);
				uint16_t buffer_length = server_packet_bufer->tot_len;
				uint16_t is_freed;
				do{
					is_freed = pbuf_free(server_packet_bufer);
				}while(is_freed != 0);
				server_packet_bufer = server_packet_bufer->next;
				if(server_packet_bufer != nullptr){
					pbuf_ref(server_packet_bufer);
				}
				tcp_recved(client_control_block, buffer_length);
			}else{
				__NOP();
				//TODO: Error Handler
			}
		}
	}

private:
	static uint8_t priority;
	struct pbuf* client_packet_buffer;
	struct tcp_pcb* client_control_block;

	static err_t accept_callback(void* arg, struct tcp_pcb* incomming_control_block, err_t error){
		tcp_setprio(incomming_control_block, priority);
		priority++;
		if(listening_sockets.contains(incomming_control_block->local_port)){
			ServerSocket* server_socket = listening_sockets[incomming_control_block->local_port];
			listening_sockets.erase(incomming_control_block->local_port);
			tcp_nagle_disable(incomming_control_block);
			tcp_arg(incomming_control_block, server_socket);
			server_socket->state = ACCEPTED;
			server_socket->client_control_block = incomming_control_block;
			server_socket->server_packet_bufer = nullptr;
			tcp_recv(incomming_control_block, receive_callback);
			tcp_err(incomming_control_block, error_callback);
			tcp_poll(incomming_control_block, poll_callback , 0);
			return ERR_OK;
		}else
			return ERROR;
	}

	static err_t receive_callback(void* arg, struct tcp_pcb* client_control_block, struct pbuf* packet_buffer, err_t error){

		ServerSocket* server_socket = (ServerSocket*) arg;
		server_socket->client_control_block = client_control_block;

		if(packet_buffer == nullptr){		//If received buffer is empty, is a signal to close connection
			server_socket->state = CLOSING;
			if(server_socket->server_packet_bufer == nullptr){			//Check if server still has to send data
				server_socket->close();
			}else{
				tcp_sent(client_control_block, send_callback);
			}
			return ERR_OK;
		}
		else if(error != ERR_OK){			//Check if packet is valid
			if(packet_buffer != nullptr){
				pbuf_free(packet_buffer);
			}
			return error;
		}
		else if(server_socket->state == ACCEPTED){
			server_socket->state = RECEIVED;
			server_socket->server_packet_bufer = packet_buffer;
			tcp_sent(client_control_block, send_callback);
			server_socket->process_data();
			pbuf_free(packet_buffer);
			return ERR_OK;
		}
		else if(server_socket->state == RECEIVED){
			if(server_socket->server_packet_bufer == nullptr){
				server_socket->server_packet_bufer = packet_buffer;
				server_socket->process_data();
			}else{
				pbuf_chain(server_socket->server_packet_bufer, packet_buffer);
			}
			pbuf_free(packet_buffer);
			return ERR_OK;
		}
		else if(server_socket->state == CLOSING){
//			tcp_recved(client_control_block, packet_buffer->tot_len);
			server_socket->server_packet_bufer = nullptr;
			pbuf_free(packet_buffer);
			return ERR_OK;
		}
		return ERR_OK;
	}

	static void error_callback(void *arg, err_t error){
		while(1){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
			HAL_Delay(500);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
			HAL_Delay(500);
		}
		//TODO: Error Handler
	}

	static err_t poll_callback(void *arg, struct tcp_pcb *client_control_block){
		ServerSocket* server_socket = (ServerSocket*)arg;
		server_socket->client_control_block = client_control_block;

		if(server_socket != nullptr){
			if(server_socket->server_packet_bufer != nullptr){
				if(server_socket->state == ACCEPTED){
					tcp_sent(client_control_block, send_callback);
				}
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

	static err_t send_callback(void *arg, struct tcp_pcb *client_control_block, u16_t len){
		ServerSocket* server_socket = (ServerSocket*)arg;
		server_socket->client_control_block = client_control_block;
		if(server_socket->server_packet_bufer != nullptr){
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

};
uint8_t ServerSocket::priority = 0;
unordered_map<uint32_t,ServerSocket*> ServerSocket::listening_sockets = {};
