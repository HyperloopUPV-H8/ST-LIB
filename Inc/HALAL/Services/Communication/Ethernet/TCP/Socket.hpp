/*
 * Socket.hpp
 *
 *  Created on: 14 nov. 2022
 *      Author: stefa
 */
#pragma once
#ifdef HAL_ETH_MODULE_ENABLED
#include "Communication/Ethernet/EthernetNode.hpp"
#include "Packets/Packet.hpp"
#include "Packets/Order.hpp"

#define PBUF_POOL_MEMORY_DESC_POSITION 8

class Socket{
public:
	enum SocketState{
		INACTIVE,
		CONNECTED,
		CLOSING
	};

	IPV4 remote_ip;
	uint32_t remote_port;
	tcp_pcb* connection_control_block;
	tcp_pcb* socket_control_block;
	SocketState state;
	pbuf* tx_packet_buffer;
	pbuf* rx_packet_buffer;
	static unordered_map<EthernetNode,Socket*> connecting_sockets;

	Socket();
	Socket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port);
	Socket(string local_ip, uint32_t local_port, string remote_ip, uint32_t remote_port);
	Socket(EthernetNode local_node, EthernetNode remote_node);


	void close();

	void reconnect();

	template<class Type, class... Types>
	bool send_order(Order<Type,Types...>& order);

	void send();

	void process_data();

	static err_t connect_callback(void* arg, struct tcp_pcb* client_control_block, err_t error);
	static err_t receive_callback(void* arg, struct tcp_pcb* client_control_block, struct pbuf* packet_buffer, err_t error);
	static err_t poll_callback(void* arg, struct tcp_pcb* client_control_block);
	static err_t send_callback(void* arg, struct tcp_pcb* client_control_block, uint16_t length);
	static void error_callback(void *arg, err_t error);

};

template<class Type, class... Types>
bool Socket::send_order(Order<Type,Types...>& order){
	if(state != CONNECTED){
		reconnect();
		return false;
	}
	struct memp* next_memory_pointer_in_packet_buffer_pool = (*(memp_pools[PBUF_POOL_MEMORY_DESC_POSITION]->tab))->next;
	if(next_memory_pointer_in_packet_buffer_pool == nullptr){
		if(socket_control_block->unsent != nullptr){
			tcp_output(socket_control_block);
		}else{
			memp_free_pool(memp_pools[PBUF_POOL_MEMORY_DESC_POSITION], next_memory_pointer_in_packet_buffer_pool);
		}
		return false;
	}

	order.build();
	if(order.buffer_size > tcp_sndbuf(socket_control_block)){
		return false;
	}

	tx_packet_buffer = pbuf_alloc(PBUF_TRANSPORT, order.buffer_size, PBUF_POOL);
	pbuf_take(tx_packet_buffer, order.buffer, order.buffer_size);
	send();
	return true;
}
#endif