/*
 * Order.hpp
 *
 *  Created on: 14 nov. 2022
 *      Author: stefa
 */
#pragma once

#include "Packet.hpp"


template<class Type, class... Types>
class Order : public Packet<Type,Types...> {
public:

    Order() = default;

    Order(int id, PacketValue<Type> value, PacketValue<Types>... rest) : Packet<Type,Types...>::Packet(id, value, rest...) {}

    Order(Packet<Type, Types...>& packet) { memcpy(this, &packet, sizeof(packet)); }

    Order(int id, void(&callback)(),PacketValue<Type> value ,PacketValue<Types>... rest) : Packet<Type, Types...>::Packet(id,value,rest...) {
        Packet<>::on_received[id] = &callback;
    }

    void set_callback(void(&callback)());
};


template<class Type, class... Types>
void Order<Type,Types...>::set_callback(void(&callback)()) {
	Packet<>::on_received[this->id] = &callback;
}
