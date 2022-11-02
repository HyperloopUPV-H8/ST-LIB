#pragma once
#include "Communication/Ethernet/ReceiversList.hpp"
#include "HALAL/Models/IPV4/IPV4.hpp"
#include "HALAL/Models/Packets/Packet.hpp"
#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "ST-LIB.hpp"

class DatagramSocket{
public:

	struct udp_pcb* udp_control_block;
	IPV4 local_ip;
	uint32_t local_port;
	IPV4 remote_ip;
	uint32_t remote_port;

	DatagramSocket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port): local_ip(local_ip), local_port(local_port), remote_ip(remote_ip), remote_port(remote_port){

		udp_control_block = udp_new();

		err_t error = udp_bind(udp_control_block, &local_ip.ip_address, local_port);

		if(error == ERR_OK)
		{
		   udp_recv(udp_control_block, receive_callback, NULL);

		   udp_connect(udp_control_block, &remote_ip.ip_address, remote_port);
		}
		else
		{
		   udp_remove(udp_control_block);
		}
	}

	~DatagramSocket(){
		close();
	}

	template<class Type, class... Types>
	void send(Packet<Type,Types...> packet){

		packet.build();

		struct pbuf* tx_buffer = pbuf_alloc(PBUF_TRANSPORT, packet.bffr_size, PBUF_RAM);

		pbuf_take(tx_buffer, packet.bffr, packet.bffr_size);

		udp_send(udp_control_block, tx_buffer);

		pbuf_free(tx_buffer);

	}

	void close(){

		udp_disconnect(udp_control_block);

		udp_remove(udp_control_block);

	}

private:
	static void receive_callback(void *args, struct udp_pcb *udp_control_block, struct pbuf *packet_buffer, const ip_addr_t *remote_address, u16_t port){

			uint8_t* received_data = (uint8_t*)packet_buffer->payload;

			uint16_t id = Packet<>::get_id(received_data);

			save_packet_by_id[id](received_data);

			pbuf_free(packet_buffer);
		}

};
