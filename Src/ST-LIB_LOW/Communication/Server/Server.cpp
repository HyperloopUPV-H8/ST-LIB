/*
 * Server.cpp
 *
 * Created on: Oct 12, 2023
 * 		Author: Ricardo
 */
#ifdef STLIB_ETH
#include "Communication/Server/Server.hpp"

vector<Server*> Server::running_servers = {};

Server::Server(IPV4 local_ip, uint32_t local_port)
    : local_ip(local_ip), local_port(local_port), status(RUNNING) {
    open_connection = new ServerSocket(local_ip, local_port);
    running_servers.push_back(this);
    running_connections = {};
    running_connections_count = 0;
}

Server::~Server() {
    if (open_connection != nullptr) {
        delete open_connection;
        open_connection = nullptr;
    }

    for (uint16_t s = 0; s < running_connections_count; s++) {
        if (running_connections[s] != nullptr) {
            delete running_connections[s];
            running_connections[s] = nullptr;
        }
    }

    auto it = find(running_servers.begin(), running_servers.end(), this);
    if (it != running_servers.end()) {
        running_servers.erase(it);
    }
}

void Server::update() {
    if (status == CLOSED) {
        return;
    }

    if (open_connection == nullptr) {
        open_connection = new ServerSocket(local_ip, local_port);
    } else if (!open_connection->is_connected() && !open_connection->is_listening()) {
        // Recover from startup/driver races where listener was not created successfully.
        delete open_connection;
        open_connection = new ServerSocket(local_ip, local_port);
    }

    if (open_connection->is_connected()) {
        if (running_connections_count < MAX_CONNECTIONS_TCP_SERVER) {
            running_connections[running_connections_count] = open_connection;
            running_connections_count++;
            open_connection = new ServerSocket(local_ip, local_port);
        } else {
            // Capacity reached: close the new connection and keep current sessions untouched.
            open_connection->close();
            delete open_connection;
            open_connection = new ServerSocket(local_ip, local_port);
        }
    }

    uint16_t write_index = 0;
    for (uint16_t s = 0; s < running_connections_count; s++) {
        ServerSocket* current = running_connections[s];
        if (current != nullptr && current->is_connected()) {
            running_connections[write_index++] = current;
        } else {
            if (current != nullptr) {
                current->close();
                delete current;
            }
        }
    }
    for (uint16_t s = write_index; s < running_connections_count; s++) {
        running_connections[s] = nullptr;
    }
    running_connections_count = write_index;

    if (status == CLOSING) {
        close_all();
    }
}

bool Server::broadcast_order(Order& order) {
    bool sent = false;
    for (uint16_t s = 0; s < running_connections_count; s++) {
        ServerSocket* connection = running_connections[s];
        if (connection != nullptr) {
            sent = connection->send_order(order) || sent;
        }
    }
    return sent;
}

void Server::close_all() {
    for (uint16_t s = 0; s < running_connections_count; s++) {
        if (running_connections[s] == nullptr) {
            continue;
        }
        running_connections[s]->close();
        delete running_connections[s];
        running_connections[s] = nullptr;
    }
    running_connections_count = 0;
    if (open_connection != nullptr) {
        open_connection->close();
        delete open_connection;
        open_connection = nullptr;
    }
    status = CLOSED;
}

uint32_t Server::connections_count() { return running_connections_count; }

void Server::update_servers() {
    for (Server* s : running_servers) {
        if (s != nullptr) {
            s->update();
        }
    }
}

#endif // STLIB_ETH
