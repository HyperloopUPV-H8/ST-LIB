#include "Packets/Packet.hpp"
#include "Packets/Order.hpp"

map<uint16_t,Order*> Order::orders = {};
map<uint16_t,Packet*> Packet::packets = {};
