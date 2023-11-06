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

/**
* @brief ServerSocket class
*/
class ServerSocket : public OrderProtocol{
public:

	enum ServerState{
		INACTIVE,
		LISTENING,
		ACCEPTED,
		CLOSING,
		CLOSED
	};

	static unordered_map<uint32_t,ServerSocket*> listening_sockets;
	struct tcp_pcb* server_control_block = nullptr;
	queue<struct pbuf*> tx_packet_buffer;
	queue<struct pbuf*> rx_packet_buffer;
	IPV4 local_ip;
	uint32_t local_port;
	IPV4 remote_ip;
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

	/**
	* @brief ends the connection between the server and the client. 
	*/
	void close();

	/**
	* @brief process the data received by the client orders. It is meant to be called only by Lwip on the receive_callback
	*
	* reads all the data received by the server in the ethernet buffer, packet by packet.
	* Then, for each packet, it processes it depending on the order id (default behavior is not process) and removes it from the buffer. 
	* This makes so the receive_callback (and thus the Socket) can only process declared orders, and ignores all other packets. 
	*/
	void process_data();

	/**
	 * @brief saves the order data into the tx_packet_buffer so it can be sent when a connection is accepted
	 *
	 * @param order the order to send, which contains the data and id of the message
	 * @return true if the data could be allocated in the buffer, false otherwise
	 */
	bool add_order_to_queue(Order& order);

	/**
	 * @brief puts the order data into the tx_packet_buffer and sends all the data in the buffer to the client
	 *
	 * @param order the order to send, which contains the data and id of the message
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

	/**
	* @brief sends all the binary data saved in the tx_packet_buffer to the connected client. 
	*/
	void send();

	/**
	* @return true if a connection with the client was established, false otherwise
	*/
	bool is_connected();

private:

	static err_t accept_callback(void* arg, struct tcp_pcb* incomming_control_block, err_t error);
	static err_t receive_callback(void* arg, struct tcp_pcb* client_control_block, struct pbuf* packet_buffer, err_t error);
	static void error_callback(void *arg, err_t error);
	static err_t poll_callback(void *arg, struct tcp_pcb *client_control_block);
	static err_t send_callback(void *arg, struct tcp_pcb *client_control_block, u16_t len);

};

#endif
