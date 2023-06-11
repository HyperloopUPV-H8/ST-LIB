#pragma once
#include "Packets/Order.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

class StateOrder : Order{
public:
	enum StateOrdersID{
		ADD_STATE_ORDERS = 5,
		REMOVE_STATE_ORDERS = 6
	};
	static OrderProtocol* informer_socket;
	static uint8_t state_orders_ids_size;
	static vector<uint16_t>* state_orders_ids;
	static StackOrder<0,uint8_t, vector<uint16_t>> add_state_orders_order;
	static StackOrder<0,uint8_t, vector<uint16_t>> remove_state_orders_order;

	static void set_socket(OrderProtocol& socket){
		informer_socket = &socket;
	}

	static void add_state_orders(vector<uint16_t>& new_ids){
		if(informer_socket == nullptr) ErrorHandler("Informer Socket has not been set");
		state_orders_ids = &new_ids;
		state_orders_ids_size = new_ids.size();
		informer_socket->send_order(add_state_orders_order);
	}

	static void remove_state_orders(vector<uint16_t>& old_ids){
		if(informer_socket == nullptr) ErrorHandler("Informer Socket has not been set");
		state_orders_ids = &old_ids;
		state_orders_ids_size = old_ids.size();
		informer_socket->send_order(remove_state_orders_order);
	}
};

OrderProtocol* StateOrder::informer_socket = nullptr;
uint8_t StateOrder::state_orders_ids_size = 0;
vector<uint16_t>* StateOrder::state_orders_ids = nullptr;
StackOrder<0,uint8_t, vector<uint16_t>> StateOrder::add_state_orders_order(StateOrdersID::ADD_STATE_ORDERS, &StateOrder::state_orders_ids_size, StateOrder::state_orders_ids);
StackOrder<0,uint8_t, vector<uint16_t>> StateOrder::remove_state_orders_order(StateOrdersID::REMOVE_STATE_ORDERS, &StateOrder::state_orders_ids_size, StateOrder::state_orders_ids);

