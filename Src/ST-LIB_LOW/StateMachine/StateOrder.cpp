#include "StateMachine/StateOrder.hpp"

OrderProtocol* StateOrder::informer_socket = nullptr;
uint8_t StateOrder::state_orders_ids_size = 0;
vector<uint16_t>* StateOrder::state_orders_ids = nullptr;
StackOrder<0,uint8_t, vector<uint16_t>> StateOrder::add_state_orders_order(StateOrdersID::ADD_STATE_ORDERS, &StateOrder::state_orders_ids_size, StateOrder::state_orders_ids);
StackOrder<0,uint8_t, vector<uint16_t>> StateOrder::remove_state_orders_order(StateOrdersID::REMOVE_STATE_ORDERS, &StateOrder::state_orders_ids_size, StateOrder::state_orders_ids);
