/*
 * Order.hpp
 *
 *  Created on: 14 nov. 2022
 *      Author: stefa
 */
#pragma once

#include "Packet.hpp"
#include "OrderProtocol.hpp"

class Order : public Packet{
public:
    virtual void set_callback(void(*callback)(void)) = 0;
    virtual void process() = 0;
    virtual void parse(OrderProtocol* socket, void* data) = 0;
    void parse(void* data) override {
    	parse(nullptr, data);
    }
    static void process_by_id(uint16_t id) {
        if (orders.find(id) != orders.end()) orders[id]->process();
    }
    static void process_data(OrderProtocol* socket, void* data) {
        uint16_t id = Packet::get_id(data);
        if (orders.contains(id)) {
            orders[id]->parse(socket, data);
            orders[id]->process();
        }
    }

protected:
    static map<uint16_t,Order*> orders;
};

template<size_t BufferLength,class... Types> requires NotCallablePack<Types*...>
class StackOrder : public StackPacket<BufferLength,Types...>, public Order{
public:
    StackOrder(uint16_t id,void(*callback)(void), Types*... values) : StackPacket<BufferLength,Types...>(id,values...), callback(callback) {orders[id] = this;}
//    StackOrder(uint16_t id, Types*... values) : StackPacket<BufferLength,Types...>(id,values...) {orders[id] = this;}
    void(*callback)(void) = nullptr;
    void set_callback(void(*callback)(void)) override {
        this->callback = callback;
    }
    void process() override {
        if (callback != nullptr) callback();
    }
    uint8_t* build() override {
        return StackPacket<BufferLength,Types...>::build();
    }
    void parse(void* data) override {
        StackPacket<BufferLength,Types...>::parse(data);
    }
    void parse(OrderProtocol* socket, void* data){
    	parse(data);
    }
    size_t get_size() override {
        return StackPacket<BufferLength,Types...>::get_size();
    }
    uint16_t get_id() override {
        return StackPacket<BufferLength,Types...>::get_id();
    }
};

#if __cpp_deduction_guides >= 201606
template<class... Types> requires NotCallablePack<Types*...>
StackOrder(uint16_t id,void(*callback)(void), Types*... values)->StackOrder<(!has_container<Types...>::value)*total_sizeof<Types...>::value, Types...>;

//template<class... Types> requires NotCallablePack<Types*...>
//StackOrder(uint16_t id, Types*... values)->StackOrder<(!has_container<Types...>::value)*total_sizeof<Types...>::value, Types...>;
#endif

class HeapOrder : public HeapPacket, public Order{
public:
    template<class... Types>
    HeapOrder(uint16_t id,void(*callback)(void), Types*... values) : HeapPacket(id,values...), callback(callback) {orders[id] = this;}

    template<class... Types>
    HeapOrder(uint16_t id, Types*... values) : HeapPacket(id,values...) {orders[id] = this;}

    void(*callback)(void) = nullptr;
    void set_callback(void(*callback)(void)) override {
        this->callback = callback;
    }
    void process() override {
        if (callback != nullptr) callback();
    }
    uint8_t* build() override {
        return HeapPacket::build();
    }
    void parse(void* data) override {
        HeapPacket::parse(data);
    }
    void parse(OrderProtocol* socket, void* data){
    	parse(data);
    }
    size_t get_size() override {
        return HeapPacket::get_size();
    }
    uint16_t get_id() override {
        return HeapPacket::get_id();
    }
};
