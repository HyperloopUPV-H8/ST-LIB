/*
 * DatagramSocket.cpp
 *
 *  Created on: 2 nov. 2022
 *      Author: stefa
 */

#include "Communication/Ethernet/UDP/DatagramSocket.hpp"

DatagramSocket::DatagramSocket() = default;

DatagramSocket::DatagramSocket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port): local_ip(local_ip), local_port(local_port), remote_ip(remote_ip), remote_port(remote_port){

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

		   //TODO: Error Handler
		}
	}

DatagramSocket::DatagramSocket(string local_ip, uint32_t local_port, string remote_ip, uint32_t remote_port): DatagramSocket(IPV4(local_ip), local_port, IPV4(remote_ip), remote_port){}

DatagramSocket::DatagramSocket(EthernetNode local_node, EthernetNode remote_node): DatagramSocket(local_node.ip, local_node.port, remote_node.ip, remote_node.port){}

DatagramSocket::~DatagramSocket(){
	close();
}

template<class Type, class... Types>
void DatagramSocket::send(Packet<Type, Types...> & packet){

	packet.build();

	struct pbuf* tx_buffer = pbuf_alloc(PBUF_TRANSPORT, packet.bffr_size, PBUF_RAM);

	pbuf_take(tx_buffer, packet.bffr, packet.bffr_size);

	udp_send(udp_control_block, tx_buffer);

	pbuf_free(tx_buffer);

}

void DatagramSocket::close(){

	udp_disconnect(udp_control_block);

	udp_remove(udp_control_block);

}


