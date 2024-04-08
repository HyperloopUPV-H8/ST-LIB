#pragma once



#include  "HALAL/Services/Communication/Ethernet/TCP/ServerSocket.hpp"

template<size_t BufferLength,class... Types> requires NotCallablePack<Types*...>
class ForwardOrder : public StackPacket<BufferLength,Types...>, public Order{
public:
    ForwardOrder(uint16_t id,ServerSocket* forwarder, Types*... values) : StackPacket<BufferLength,Types...>(id,values...){
        orders[id] = this;
        forwarding_sockets->push_back(forwarder);
    }
    ForwardOrder(uint16_t id,ServerSocket* forwarder,void(*callback)(void), Types*... values) : StackPacket<BufferLength,Types...>(id,values...),callback(callback){
        orders[id] = this;
        forwarding_sockets->push_back(forwarder);
        }
    ForwardOrder(uint16_t id,vector<ServerSocket*>& forwarders,Types*... values) : StackPacket<BufferLength,Types...>(id,values...) ,forwarding_sockets{&forwarders}{orders[id] = this;}
    ForwardOrder(uint16_t id,vector<ServerSocket*>& forwarders,void(*callback)(void), Types*... values) : StackPacket<BufferLength,Types...>(id,values...),callback(callback),forwarding_sockets{&forwarders}{orders[id] = this;}
    void(*callback)(void) = nullptr;

    void process() override {
        if(callback) callback();
    }

    void parse(void* data) override {
        return;
    }
    void parse(OrderProtocol* socket, void* data) override{
    	struct pbuf* packet = pbuf_alloc(PBUF_TRANSPORT, get_size(), PBUF_POOL);
        pbuf_take(packet, data, get_size());
        for(const auto& forwarding_socket : *forwarding_sockets)
        {
            forwarding_socket->tx_packet_buffer.push(packet);
            forwarding_socket->send();
        }
    }
    size_t get_size() override {
        return StackPacket<BufferLength,Types...>::get_size();
    }
    uint16_t get_id() override {
        return StackPacket<BufferLength,Types...>::get_id();
    }


private:
    void set_callback(void(*callback)(void))override{
        return;
    }
    uint8_t* build() override {
        return StackPacket<BufferLength,Types...>::build();
    }
    void set_pointer(size_t index, void* pointer) override{
		StackPacket<BufferLength,Types...>::set_pointer(index, pointer);
	}
    // socket(s) in charge of forwarding the order
    vector<ServerSocket*>* forwarding_sockets{};
};

#if __cpp_deduction_guides >= 201606
template<class... Types> requires NotCallablePack<Types*...>
ForwardOrder(uint16_t id,ServerSocket& fwd, Types*... values)->ForwardOrder<(!has_container<Types...>::value)*total_sizeof<Types...>::value, Types...>;
template<class... Types> requires NotCallablePack<Types*...>
ForwardOrder(uint16_t id,ServerSocket& fwd,void(*callback)(void), Types*... values)->ForwardOrder<(!has_container<Types...>::value)*total_sizeof<Types...>::value, Types...>;
template<class... Types> requires NotCallablePack<Types*...>
ForwardOrder(uint16_t id,vector<ServerSocket*> forwarders, Types*... values)->ForwardOrder<(!has_container<Types...>::value)*total_sizeof<Types...>::value, Types...>;
template<class... Types> requires NotCallablePack<Types*...>
ForwardOrder(uint16_t id,vector<ServerSocket*> forwarders, void(*callback)(void),Types*... values)->ForwardOrder<(!has_container<Types...>::value)*total_sizeof<Types...>::value, Types...>;
#endif