/*
 * DatagramSocket.cpp
 *
 *  Created on: 2 nov. 2022
 *      Author: stefa
 */

#include "Communication/Ethernet/UDP/DatagramSocket.hpp"

map<decltype(Packet<>::id), function<void(uint8_t*)>> Packet<>::save_by_id = {};
map<decltype(Packet<>::id), void(*)()> Packet<>::on_received = {};

DatagramSocket::DatagramSocket() = default;

DatagramSocket::DatagramSocket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port): local_ip(local_ip), local_port(local_port), remote_ip(remote_ip), remote_port(remote_port){

		udp_control_block = udp_new();
		err_t error = udp_bind(udp_control_block, &local_ip.address, local_port);

		if(error == ERR_OK){
		   udp_recv(udp_control_block, receive_callback, nullptr);
		   udp_connect(udp_control_block, &remote_ip.address, remote_port);
		}
		else{
		   udp_remove(udp_control_block);
		   //TODO: Error Handler
		}
	}

DatagramSocket::DatagramSocket(string local_ip, uint32_t local_port, string remote_ip, uint32_t remote_port): DatagramSocket(IPV4(local_ip), local_port, IPV4(remote_ip), remote_port){}

DatagramSocket::DatagramSocket(EthernetNode local_node, EthernetNode remote_node): DatagramSocket(local_node.ip, local_node.port, remote_node.ip, remote_node.port){}

DatagramSocket::~DatagramSocket(){
	close();
}

void DatagramSocket::close(){
	udp_disconnect(udp_control_block);
	udp_remove(udp_control_block);
}

void DatagramSocket::receive_callback(void *args, struct udp_pcb *udp_control_block, struct pbuf *packet_buffer, const ip_addr_t *remote_address, u16_t port){
	uint8_t* received_data = (uint8_t*)packet_buffer->payload;
	uint16_t id = Packet<>::get_id(received_data);

	if(Packet<>::save_by_id.contains(id)){
		Packet<>::save_by_id[id](received_data);
	}

	pbuf_free(packet_buffer);
}


