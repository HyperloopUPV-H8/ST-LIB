/*
 * EthernetNode.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */
#include "Communication/Ethernet/EthernetNode.hpp"
#ifdef HAL_ETH_MODULE_ENABLED

static constexpr uint16_t UDP_PORT = 50400;
static constexpr uint16_t TCP_SERVER_PORT = 50500;
static constexpr uint16_t TCP_CLIENT_PORT = 50501;
static constexpr uint16_t TFTP = 69;
static constexpr uint16_t SNTP = 123;

EthernetNode::EthernetNode(IPV4 ip, uint16_t port):ip(ip), port(port){}
EthernetNode::EthernetNode(IPV4 ip, uint16_t port, HostType type):ip(ip), port(port), type(type){}

bool EthernetNode::operator==(const EthernetNode& other) const{
	return ip.address.addr == other.ip.address.addr && port == other.port;
}

std::size_t hash<EthernetNode>::operator()(const EthernetNode& key) const
{
  using std::size_t;
  using std::hash;
  using std::string;

  return (hash<uint16_t>()(key.ip.address.addr)) ^ (hash<uint16_t>()(key.port) << 1);
}

IPV4 VCU_IP("192.168.1.2");
EthernetNode VCU_TCP_CLIENT(VCU_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode VCU_TCP_SERVER(VCU_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode VCU_UDP(VCU_IP, UDP_PORT);

IPV4 TCU_IP("192.168.1.3");
EthernetNode TCU_TCP_CLIENT(TCU_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode TCU_TCP_SERVER(TCU_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode TCU_UDP(TCU_IP, UDP_PORT);

IPV4 BLCU_IP("192.168.1.4");
EthernetNode BLCU_TCP_CLIENT(BLCU_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode BLCU_TCP_SERVER(BLCU_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode BLCU_UDP(BLCU_IP, UDP_PORT);

IPV4 LCU_MASTER_IP("192.168.1.5");
EthernetNode LCU_MASTER_TCP_CLIENT(LCU_MASTER_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode LCU_MASTER_TCP_SERVER(LCU_MASTER_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode LCU_MASTER_UDP(LCU_MASTER_IP, UDP_PORT);

IPV4 LCU_SLAVE_IP("192.168.1.6");
EthernetNode LCU_SLAVE_TCP_CLIENT(LCU_SLAVE_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode LCU_SLAVE_TCP_SERVER(LCU_SLAVE_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode LCU_SLAVE_UDP(LCU_SLAVE_IP, UDP_PORT);

IPV4 PCU_IP("192.168.1.7");
EthernetNode PCU_TCP_CLIENT(PCU_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode PCU_TCP_SERVER(PCU_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode PCU_UDP(PCU_IP, UDP_PORT);

IPV4 BMSA_IP("192.168.1.8");
EthernetNode BMSA_TCP_CLIENT(BMSA_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode BMSA_TCP_SERVER(BMSA_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode BMSA_UDP(BMSA_IP, UDP_PORT);

IPV4 OBCCU_IP("192.168.1.9");
EthernetNode OBCCU_TCP_CLIENT(OBCCU_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode OBCCU_TCP_SERVER(OBCCU_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode OBCCU_UDP(OBCCU_IP, UDP_PORT);

IPV4 Backend_IP("192.168.1.11");
EthernetNode Backend_TCP_CLIENT(Backend_IP, TCP_CLIENT_PORT,EthernetNode::HostType::Client);
EthernetNode Backend_TCP_SERVER(Backend_IP, TCP_SERVER_PORT,EthernetNode::HostType::Server);
EthernetNode Backend_UDP(Backend_IP, UDP_PORT);

map<Board::ID, EthernetNode> EthernetNode::udp_nodes = {{Board::VCU, VCU_UDP},{Board::TCU, TCU_UDP},{Board::BLCU, BLCU_UDP},{Board::LCU_MASTER, LCU_MASTER_UDP},{Board::LCU_SLAVE, LCU_SLAVE_UDP},{Board::PCU,PCU_UDP},{Board::OBCCU,OBCCU_UDP},{Board::BMSA,BMSA_UDP}};
map<Board::ID, EthernetNode> EthernetNode::tcp_client_nodes = {{Board::VCU, VCU_TCP_CLIENT},{Board::TCU, TCU_TCP_CLIENT},{Board::BLCU, BLCU_TCP_CLIENT},{Board::LCU_MASTER, LCU_MASTER_TCP_CLIENT},{Board::LCU_SLAVE, LCU_SLAVE_TCP_CLIENT},{Board::PCU,PCU_TCP_CLIENT},{Board::OBCCU,OBCCU_TCP_CLIENT},{Board::BMSA,BMSA_TCP_CLIENT}};
map<Board::ID, EthernetNode> EthernetNode::tcp_server_nodes = {{Board::VCU, VCU_TCP_SERVER},{Board::TCU, TCU_TCP_SERVER},{Board::BLCU, BLCU_TCP_SERVER},{Board::LCU_MASTER, LCU_MASTER_TCP_SERVER},{Board::LCU_SLAVE, LCU_SLAVE_TCP_SERVER},{Board::PCU,PCU_TCP_SERVER},{Board::OBCCU,OBCCU_TCP_SERVER},{Board::BMSA,BMSA_TCP_SERVER}};

#endif
