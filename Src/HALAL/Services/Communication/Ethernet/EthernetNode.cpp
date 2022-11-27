/*
 * EthernetNode.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */

#include "Communication/Ethernet/EthernetNode.hpp"

EthernetNode::EthernetNode(IPV4 ip, uint32_t port):ip(ip), port(port){}

bool EthernetNode::operator==(const EthernetNode& other) const{
	return ip.ip_address.addr == other.ip.ip_address.addr && port == other.port;
}

std::size_t hash<EthernetNode>::operator()(const EthernetNode& key) const
{
  using std::size_t;
  using std::hash;
  using std::string;

  return (hash<uint32_t>()(key.ip.ip_address.addr)) ^ (hash<uint32_t>()(key.port) << 1);
}

