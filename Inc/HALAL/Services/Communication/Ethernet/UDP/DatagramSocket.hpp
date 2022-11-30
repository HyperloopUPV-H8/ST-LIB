#pragma once

#include "Communication/Ethernet/EthernetNode.hpp"
#include "Packets/Packet.hpp"

#define PBUF_POOL_MEMORY_DESC_POSITION 8

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
	bool send(Packet<Type,Types...>& packet);

	void close();

private:
	static void receive_callback(void *args, struct udp_pcb *udp_control_block, struct pbuf *packet_buffer, const ip_addr_t *remote_address, u16_t port);
	
};

template<class Type, class... Types>
bool DatagramSocket::send(Packet<Type, Types...> & packet){
	packet.build();

	struct pbuf* tx_buffer = pbuf_alloc(PBUF_TRANSPORT, packet.buffer_size, PBUF_RAM);
	pbuf_take(tx_buffer, packet.buffer, packet.buffer_size);
	udp_send(udp_control_block, tx_buffer);
	pbuf_free(tx_buffer);

	return true;
}



