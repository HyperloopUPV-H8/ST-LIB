#pragma once
#include "Packets/Order.hpp"

class OrderProtocol{
public:
    virtual bool send_order(Order& order) = 0;
    static vector<OrderProtocol*> sockets;
};