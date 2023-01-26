/*
 * ServerSocket.hpp
 *
 *  Created on: 14 nov. 2022
 *      Author: stefa
 */
#pragma once
//#ifdef HAL_ETH_MODULE_ENABLED
#include "Communication/Ethernet/EthernetNode.hpp"
#include "Packets/Packet.hpp"
#include "Packets/Order.hpp"

#define PBUF_POOL_MEMORY_DESC_POSITION 8

class ServerSocket{
public:

	enum ServerState{
		INACTIVE,
		LISTENING,
		ACCEPTED,
		CLOSING
	};

	static unordered_map<uint32_t,ServerSocket*> listening_sockets;
	struct tcp_pcb* server_control_block;
	struct pbuf* tx_packet_buffer;
	struct pbuf* rx_packet_buffer;
	IPV4 local_ip;
	uint32_t local_port;
	ServerState state;
	static uint8_t priority;
	struct tcp_pcb* client_control_block;


	ServerSocket();
	ServerSocket(IPV4 local_ip, uint32_t local_port);
	ServerSocket(string local_ip, uint32_t local_port);
	ServerSocket(EthernetNode local_node);
	~ServerSocket();

	void close();

	void process_data();

	template<class Type, class...Types>
	bool send_order(Order<Type,Types...>& order);

	void send();

private:

	static err_t accept_callback(void* arg, struct tcp_pcb* incomming_control_block, err_t error);
	static err_t receive_callback(void* arg, struct tcp_pcb* client_control_block, struct pbuf* packet_buffer, err_t error);
	static void error_callback(void *arg, err_t error);
	static err_t poll_callback(void *arg, struct tcp_pcb *client_control_block);
	static err_t send_callback(void *arg, struct tcp_pcb *client_control_block, u16_t len);

};

template<class Type, class...Types>
bool ServerSocket::send_order(Order<Type,Types...>& order){
	if(state != ACCEPTED){
		return false;
	}
	struct memp* next_memory_pointer_in_packet_buffer_pool = (*(memp_pools[PBUF_POOL_MEMORY_DESC_POSITION]->tab))->next;
	if(next_memory_pointer_in_packet_buffer_pool == nullptr){
		if(client_control_block->unsent != nullptr){
			tcp_output(client_control_block);
		}else{
			memp_free_pool(memp_pools[PBUF_POOL_MEMORY_DESC_POSITION], next_memory_pointer_in_packet_buffer_pool);
		}
		return false;
	}

	order.build();
	if(order.bffr_size > tcp_sndbuf(client_control_block)){
		return false;
	}

	tx_packet_buffer = pbuf_alloc(PBUF_TRANSPORT, order.bffr_size, PBUF_POOL);
	pbuf_take(tx_packet_buffer, order.bffr, order.bffr_size);
	send();
	return true;
}
//#endif
