#pragma once
#include "Communication/Ethernet/EthernetNode.hpp"
#include "Communication/Ethernet/Ethernet.hpp"
#include "Packets/Packet.hpp"

#ifdef HAL_ETH_MODULE_ENABLED
#define PBUF_POOL_MEMORY_DESC_POSITION 8

class DatagramSocket{
public:

	struct udp_pcb* udp_control_block;
	IPV4 local_ip;
	uint32_t local_port;
	IPV4 remote_ip;
	uint32_t remote_port;
	bool is_disconnected = true;

	DatagramSocket();
	DatagramSocket(DatagramSocket&& other);
	DatagramSocket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port);
	DatagramSocket(EthernetNode local_node, EthernetNode remote_node);
	~DatagramSocket();

	void operator=(DatagramSocket&&);

	bool send(Packet& packet){
		uint8_t* packet_buffer = packet.build();

		struct pbuf* tx_buffer = pbuf_alloc(PBUF_TRANSPORT, packet.size, PBUF_RAM);
		pbuf_take(tx_buffer, packet_buffer, packet.size);
		udp_send(udp_control_block, tx_buffer);
		pbuf_free(tx_buffer);

		return true;
	}

	void reconnect();

	void close();

private:
	static void receive_callback(void *args, struct udp_pcb *udp_control_block, struct pbuf *packet_buffer, const ip_addr_t *remote_address, u16_t port);
	
};


#endif
