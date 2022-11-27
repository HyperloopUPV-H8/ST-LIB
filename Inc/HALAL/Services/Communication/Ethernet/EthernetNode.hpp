#pragma once

#include "IPV4/IPV4.hpp"

struct EthernetNode{
	IPV4 ip;
	uint32_t port;

	EthernetNode(IPV4 ip, uint32_t port);

	bool operator==(const EthernetNode& other) const;
};

namespace std {

  template <>
  struct hash<EthernetNode>
  {
    std::size_t operator()(const EthernetNode& k) const;
  };

}
