#pragma once

#include "IPV4/IPV4.hpp"
#include "BoardID/BoardID.hpp"

#ifdef HAL_ETH_MODULE_ENABLED

struct EthernetNode{
	enum HostType{
		Client,
		Server
	};

	IPV4 ip;
	uint16_t port;
	HostType type = Client;
	static map<Board::ID, EthernetNode> udp_nodes;
	static map<Board::ID, EthernetNode> tcp_client_nodes;
	static map<Board::ID, EthernetNode> tcp_server_nodes;

	EthernetNode(IPV4 ip, uint16_t port);
	EthernetNode(IPV4 ip, uint16_t port, HostType type);

	bool operator==(const EthernetNode& other) const;
};

namespace std {

  template <>
  struct hash<EthernetNode>
  {
    std::size_t operator()(const EthernetNode& k) const;
  };

}

#endif
