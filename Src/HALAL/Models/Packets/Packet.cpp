#include "Packets/Packet.hpp"

map<decltype(Packet<>::id), function<void(uint8_t*)>> Packet<>::save_by_id = {};
map<decltype(Packet<>::id), void(*)()> Packet<>::on_received = {};
