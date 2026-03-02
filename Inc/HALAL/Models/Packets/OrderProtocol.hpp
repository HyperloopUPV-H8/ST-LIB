#pragma once
#include "HALAL/Models/Packets/Packet.hpp"

class Order;

class OrderProtocol {
public:
    virtual ~OrderProtocol() = default;
    virtual bool send_order(Order& order) = 0;
    static vector<OrderProtocol*> sockets;

    friend class ServerSocket;
    friend class Socket;
};
