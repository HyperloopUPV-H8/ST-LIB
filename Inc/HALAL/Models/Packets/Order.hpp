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

    Order();
    Order(int id, PacketValue<Type> value, PacketValue<Types>... rest);
    Order(Packet<Type, Types...>& packet);
    Order(int id, void(&callback)(),PacketValue<Type> value ,PacketValue<Types>... rest);

    void set_callback(void(&callback)());
};

template<class Type, class... Types>
Order<Type,Types...>::Order() = default;

template<class Type, class... Types>
Order<Type,Types...>::Order(int id, PacketValue<Type> value, PacketValue<Types>... rest) : Packet<Type,Types...>::Packet(id, value, rest...) {}

template<class Type, class... Types>
Order<Type,Types...>::Order(Packet<Type, Types...>& packet){
	memcpy(this, &packet, sizeof(packet));
}

template<class Type, class... Types>
Order<Type,Types...>::Order(int id, void(&callback)(),PacketValue<Type> value ,PacketValue<Types>... rest) : Packet<Type, Types...>::Packet(id,value,rest...) {
    Packet<>::on_received[id] = &callback;
}

template<class Type, class... Types>
void Order<Type,Types...>::set_callback(void(&callback)()) {
	Packet<>::on_received[this->id] = &callback;
}
