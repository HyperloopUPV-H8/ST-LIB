/*
 * ServerSocket.hpp
 *
 *  Created on: 14 nov. 2022
 *      Author: stefa
 */
#pragma once

#include "Communication/Ethernet/EthernetNode.hpp"
#include "Communication/Ethernet/Ethernet.hpp"
#include "Packets/Packet.hpp"
#include "Packets/Order.hpp"
#include "Packets/OrderProtocol.hpp"
#ifdef HAL_ETH_MODULE_ENABLED

#define PBUF_POOL_MEMORY_DESC_POSITION 8

class ServerSocket : public OrderProtocol{
public:

	enum ServerState{
		INACTIVE,
		LISTENING,
		ACCEPTED,
		CLOSING
	};

	static unordered_map<uint32_t,ServerSocket*> listening_sockets;
	static queue<ServerSocket*> sockets_to_flush;
	struct tcp_pcb* server_control_block = nullptr;
	queue<struct pbuf*> tx_packet_buffer;
	queue<struct pbuf*> rx_packet_buffer;
	IPV4 local_ip;
	uint32_t local_port;
	ServerState state;
	static uint8_t priority;
	struct tcp_pcb* client_control_block;


	ServerSocket();
	ServerSocket(ServerSocket&& other);
	ServerSocket(IPV4 local_ip, uint32_t local_port);
	ServerSocket(string local_ip, uint32_t local_port);
	ServerSocket(EthernetNode local_node);
	~ServerSocket();

	void operator=(ServerSocket&& other);

	void close();

	void process_data();

	/*
	 * @brief puts the order data into the tx_packet_buffer so it can be sent when a connection is accepted
	 * @return true if the data could be allocated in the buffer, false otherwise
	 */
	bool queue_order(Order& order);

	/*
	 * @brief puts the order data into the tx_packet_buffer and sends it
	 * @return true if the data was sent successfully, false otherwise
	 */
	bool send_order(Order& order) override{
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

		uint8_t* order_buffer = order.build();
		if(order.get_size() > tcp_sndbuf(client_control_block)){
			return false;
		}

		struct pbuf* packet = pbuf_alloc(PBUF_TRANSPORT, order.get_size(), PBUF_POOL);
		pbuf_take(packet, order_buffer, order.get_size());
		tx_packet_buffer.push(packet);
		send();
		return true;
	}

	void send();

	bool is_connected();

private:

	static err_t accept_callback(void* arg, struct tcp_pcb* incomming_control_block, err_t error);
	static err_t receive_callback(void* arg, struct tcp_pcb* client_control_block, struct pbuf* packet_buffer, err_t error);
	static void error_callback(void *arg, err_t error);
	static err_t poll_callback(void *arg, struct tcp_pcb *client_control_block);
	static err_t send_callback(void *arg, struct tcp_pcb *client_control_block, u16_t len);

};

#endif
