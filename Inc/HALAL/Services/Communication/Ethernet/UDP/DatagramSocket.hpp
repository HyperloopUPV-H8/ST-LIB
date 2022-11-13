#pragma once

#include "ST-LIB.hpp"

class DatagramSocket{
public:

	struct udp_pcb* udp_control_block;
	IPV4 local_ip;
	uint32_t local_port;
	IPV4 remote_ip;
	uint32_t remote_port;

	DatagramSocket();

	DatagramSocket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port);

	DatagramSocket(string local_ip, uint32_t local_port, string remote_ip, uint32_t remote_port);

	DatagramSocket(EthernetNode local_node, EthernetNode remote_node);

	~DatagramSocket();

	template<class Type, class... Types>
	void send(Packet<Type,Types...>& packet);

	void close();

private:
	static void receive_callback(void *args, struct udp_pcb *udp_control_block, struct pbuf *packet_buffer, const ip_addr_t *remote_address, u16_t port){

		uint8_t* received_data = (uint8_t*)packet_buffer->payload;

		uint16_t id = Packet<>::get_id(received_data);

		save_packet_by_id[id](received_data);

		pbuf_free(packet_buffer);
	}

};


