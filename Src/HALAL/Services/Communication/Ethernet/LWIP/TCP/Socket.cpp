/*
 * Socket.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */
#include "HALAL/Services/Communication/Ethernet/LWIP/TCP/Socket.hpp"
#include "HALAL/Services/Communication/Ethernet/LWIP/TCP/TcpOrderStreamParser.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#ifdef HAL_ETH_MODULE_ENABLED

unordered_map<EthernetNode, Socket*> Socket::connecting_sockets = {};

Socket::Socket() = default;

Socket::Socket(Socket&& other)
    : connection_control_block(other.connection_control_block),
      socket_control_block(other.socket_control_block),
      tx_packet_buffer(move(other.tx_packet_buffer)),
      rx_packet_buffer(move(other.rx_packet_buffer)),
      rx_stream_buffer(move(other.rx_stream_buffer)), local_ip(move(other.local_ip)),
      local_port(other.local_port), remote_ip(move(other.remote_ip)),
      remote_port(other.remote_port), state(other.state),
      pending_connection_reset(other.pending_connection_reset),
      connect_poll_ticks(other.connect_poll_ticks), use_keep_alives(other.use_keep_alives),
      keepalive_config(other.keepalive_config) {
    other.connection_control_block = nullptr;
    other.socket_control_block = nullptr;
    other.state = INACTIVE;
    other.pending_connection_reset = false;
    other.connect_poll_ticks = 0;

    if (connection_control_block != nullptr) {
        tcp_arg(connection_control_block, this);
    }
    if (socket_control_block != nullptr) {
        tcp_arg(socket_control_block, this);
    }

    EthernetNode remote_node(remote_ip, remote_port);
    auto it = connecting_sockets.find(remote_node);
    if (it != connecting_sockets.end() && it->second == &other) {
        it->second = this;
    }

    if (std::find(OrderProtocol::sockets.begin(), OrderProtocol::sockets.end(), this) ==
        OrderProtocol::sockets.end()) {
        OrderProtocol::sockets.push_back(this);
    }
}

void Socket::operator=(Socket&& other) {
    if (this == &other) {
        return;
    }
    close();

    connection_control_block = other.connection_control_block;
    socket_control_block = other.socket_control_block;
    tx_packet_buffer = move(other.tx_packet_buffer);
    rx_packet_buffer = move(other.rx_packet_buffer);
    rx_stream_buffer = move(other.rx_stream_buffer);
    local_ip = move(other.local_ip);
    local_port = other.local_port;
    remote_ip = move(other.remote_ip);
    remote_port = other.remote_port;
    state = other.state;
    pending_connection_reset = other.pending_connection_reset;
    connect_poll_ticks = other.connect_poll_ticks;
    use_keep_alives = other.use_keep_alives;
    keepalive_config = other.keepalive_config;

    other.connection_control_block = nullptr;
    other.socket_control_block = nullptr;
    other.state = INACTIVE;
    other.pending_connection_reset = false;
    other.connect_poll_ticks = 0;

    if (connection_control_block != nullptr) {
        tcp_arg(connection_control_block, this);
    }
    if (socket_control_block != nullptr) {
        tcp_arg(socket_control_block, this);
    }

    EthernetNode remote_node(remote_ip, remote_port);
    auto it = connecting_sockets.find(remote_node);
    if (it != connecting_sockets.end() && it->second == &other) {
        it->second = this;
    }

    if (std::find(OrderProtocol::sockets.begin(), OrderProtocol::sockets.end(), this) ==
        OrderProtocol::sockets.end()) {
        OrderProtocol::sockets.push_back(this);
    }
}

Socket::~Socket() {
    close();

    for (auto it = connecting_sockets.begin(); it != connecting_sockets.end();) {
        if (it->second == this) {
            it = connecting_sockets.erase(it);
        } else {
            ++it;
        }
    }

    auto it = std::find(OrderProtocol::sockets.begin(), OrderProtocol::sockets.end(), this);
    if (it != OrderProtocol::sockets.end()) {
        OrderProtocol::sockets.erase(it);
    }
}

Socket::Socket(
    IPV4 local_ip,
    uint32_t local_port,
    IPV4 remote_ip,
    uint32_t remote_port,
    bool use_keep_alive
)
    : local_ip(local_ip), local_port(local_port), remote_ip(remote_ip), remote_port(remote_port),
      use_keep_alives{use_keep_alive} {
    if (not Ethernet::is_running) {
        ErrorHandler("Cannot declare TCP socket before Ethernet::start()");
        return;
    }
    state = INACTIVE;
    tx_packet_buffer = {};
    rx_packet_buffer = {};
    rx_stream_buffer = {};
    rx_stream_buffer.reserve(TcpOrderStreamParser::MAX_RX_STREAM_BUFFER_BYTES);
    EthernetNode remote_node(remote_ip, remote_port);

    connection_control_block = tcp_new();
    if (connection_control_block == nullptr) {
        ErrorHandler("Cannot allocate TCP control block");
        return;
    }
    ip_set_option(connection_control_block, SOF_REUSEADDR);

    err_t bind_error = tcp_bind(connection_control_block, &local_ip.address, local_port);
    if (bind_error != ERR_OK) {
        tcp_abort(connection_control_block);
        connection_control_block = nullptr;
        ErrorHandler("Cannot bind TCP socket. Error code: %d", bind_error);
        return;
    }
    tcp_nagle_disable(connection_control_block);
    tcp_arg(connection_control_block, this);
    tcp_poll(connection_control_block, connection_poll_callback, 1);
    tcp_err(connection_control_block, connection_error_callback);

    connecting_sockets[remote_node] = this;
    err_t connect_error =
        tcp_connect(connection_control_block, &remote_ip.address, remote_port, connect_callback);
    if (connect_error != ERR_OK && connect_error != ERR_ISCONN) {
        connecting_sockets.erase(remote_node);
        tcp_abort(connection_control_block);
        connection_control_block = nullptr;
        ErrorHandler("Cannot connect TCP socket. Error code: %d", connect_error);
        return;
    }

    if (std::find(OrderProtocol::sockets.begin(), OrderProtocol::sockets.end(), this) ==
        OrderProtocol::sockets.end()) {
        OrderProtocol::sockets.push_back(this);
    }
}

Socket::Socket(
    IPV4 local_ip,
    uint32_t local_port,
    IPV4 remote_ip,
    uint32_t remote_port,
    uint32_t inactivity_time_until_keepalive_ms,
    uint32_t space_between_tries_ms,
    uint32_t tries_until_disconnection
)
    : Socket(local_ip, local_port, remote_ip, remote_port) {
    keepalive_config.inactivity_time_until_keepalive_ms = inactivity_time_until_keepalive_ms;
    keepalive_config.space_between_tries_ms = space_between_tries_ms;
    keepalive_config.tries_until_disconnection = tries_until_disconnection;
}

Socket::Socket(EthernetNode local_node, EthernetNode remote_node)
    : Socket(local_node.ip, local_node.port, remote_node.ip, remote_node.port) {}

void Socket::clear_packet_queues() {
    while (!tx_packet_buffer.empty()) {
        pbuf* packet = tx_packet_buffer.front();
        tx_packet_buffer.pop();
        if (packet != nullptr) {
            pbuf_free(packet);
        }
    }
    while (!rx_packet_buffer.empty()) {
        pbuf* packet = rx_packet_buffer.front();
        rx_packet_buffer.pop();
        if (packet != nullptr) {
            pbuf_free(packet);
        }
    }
}

void Socket::close() {
    if (socket_control_block != nullptr) {
        tcp_arg(socket_control_block, nullptr);
        tcp_sent(socket_control_block, nullptr);
        tcp_recv(socket_control_block, nullptr);
        tcp_err(socket_control_block, nullptr);
        tcp_poll(socket_control_block, nullptr, 0);

        err_t close_error = tcp_close(socket_control_block);
        if (close_error != ERR_OK) {
            tcp_abort(socket_control_block);
        }
    }
    if (connection_control_block != nullptr && connection_control_block != socket_control_block) {
        tcp_arg(connection_control_block, nullptr);
        tcp_poll(connection_control_block, nullptr, 0);
        tcp_err(connection_control_block, nullptr);
        tcp_abort(connection_control_block);
    }
    socket_control_block = nullptr;
    connection_control_block = nullptr;
    clear_packet_queues();
    rx_stream_buffer.clear();
    connect_poll_ticks = 0;
    pending_connection_reset = false;
    state = INACTIVE;

    EthernetNode remote_node(remote_ip, remote_port);
    auto it = connecting_sockets.find(remote_node);
    if (it != connecting_sockets.end() && it->second == this) {
        connecting_sockets.erase(it);
    }
}

void Socket::reconnect() {
    if (state == CONNECTED) {
        return;
    }
    if (pending_connection_reset || connection_control_block == nullptr) {
        reset();
        return;
    }

    if (connection_control_block->state == SYN_SENT) {
        return;
    }

    EthernetNode remote_node(remote_ip, remote_port);
    connecting_sockets[remote_node] = this;

    err_t connect_error =
        tcp_connect(connection_control_block, &remote_ip.address, remote_port, connect_callback);
    if (connect_error != ERR_OK && connect_error != ERR_ISCONN) {
        pending_connection_reset = true;
    }
}

void Socket::reset() {
    state = INACTIVE;
    socket_control_block = nullptr;
    clear_packet_queues();
    rx_stream_buffer.clear();
    connect_poll_ticks = 0;

    if (connection_control_block != nullptr) {
        tcp_arg(connection_control_block, nullptr);
        tcp_poll(connection_control_block, nullptr, 0);
        tcp_err(connection_control_block, nullptr);
        tcp_abort(connection_control_block);
    }
    connection_control_block = tcp_new();
    if (connection_control_block == nullptr) {
        pending_connection_reset = true;
        return;
    }
    ip_set_option(connection_control_block, SOF_REUSEADDR);

    err_t bind_error = tcp_bind(connection_control_block, &local_ip.address, local_port);
    if (bind_error != ERR_OK) {
        tcp_abort(connection_control_block);
        connection_control_block = nullptr;
        pending_connection_reset = true;
        return;
    }
    tcp_nagle_disable(connection_control_block);
    tcp_arg(connection_control_block, this);
    tcp_poll(connection_control_block, connection_poll_callback, 1);
    tcp_err(connection_control_block, connection_error_callback);

    EthernetNode remote_node(remote_ip, remote_port);
    connecting_sockets[remote_node] = this;

    err_t connect_error =
        tcp_connect(connection_control_block, &remote_ip.address, remote_port, connect_callback);
    pending_connection_reset = !(connect_error == ERR_OK || connect_error == ERR_ISCONN);
}

void Socket::send() {
    if (state != CONNECTED || socket_control_block == nullptr) {
        return;
    }

    while (!tx_packet_buffer.empty()) {
        pbuf* temporal_packet_buffer = tx_packet_buffer.front();
        if (temporal_packet_buffer == nullptr) {
            tx_packet_buffer.pop();
            continue;
        }

        if (temporal_packet_buffer->tot_len > tcp_sndbuf(socket_control_block)) {
            break;
        }

        err_t error = tcp_write(
            socket_control_block,
            temporal_packet_buffer->payload,
            temporal_packet_buffer->tot_len,
            TCP_WRITE_FLAG_COPY
        );
        if (error == ERR_OK) {
            tx_packet_buffer.pop();
            pbuf_free(temporal_packet_buffer);
        } else if (error == ERR_MEM) {
            break;
        } else {
            state = CLOSING;
            break;
        }
    }
    if (socket_control_block != nullptr) {
        tcp_output(socket_control_block);
    }
}

void Socket::process_data() {
    while (!rx_packet_buffer.empty()) {
        pbuf* packet = rx_packet_buffer.front();
        rx_packet_buffer.pop();
        if (packet == nullptr) {
            continue;
        }

        if (socket_control_block != nullptr) {
            tcp_recved(socket_control_block, packet->tot_len);
        }

        const size_t previous_size = rx_stream_buffer.size();
        const size_t append_size = packet->tot_len;
        rx_stream_buffer.resize(previous_size + append_size);
        if (pbuf_copy_partial(
                packet,
                rx_stream_buffer.data() + previous_size,
                packet->tot_len,
                0
            ) == static_cast<u16_t>(packet->tot_len)) {
            TcpOrderStreamParser::process(this, remote_ip, rx_stream_buffer);
        } else {
            rx_stream_buffer.resize(previous_size);
        }
        pbuf_free(packet);
    }
}

bool Socket::add_order_to_queue(Order& order) {
    if (state != Socket::SocketState::CONNECTED || socket_control_block == nullptr) {
        return false;
    }

    if (tx_packet_buffer.size() >= MAX_TX_QUEUE_DEPTH) {
        return false;
    }

    const size_t order_size = order.get_size();
    if (order_size == 0 || order_size > tcp_sndbuf(socket_control_block)) {
        return false;
    }

    uint8_t* order_buffer = order.build();
    pbuf* packet = pbuf_alloc(PBUF_TRANSPORT, order_size, PBUF_RAM);
    if (packet == nullptr) {
        return false;
    }
    if (pbuf_take(packet, order_buffer, order_size) != ERR_OK) {
        pbuf_free(packet);
        return false;
    }
    Socket::tx_packet_buffer.push(packet);
    return true;
}

bool Socket::is_connected() { return state == Socket::SocketState::CONNECTED; }

err_t Socket::connect_callback(void* arg, struct tcp_pcb* client_control_block, err_t error) {
    Socket* socket = static_cast<Socket*>(arg);
    if (socket == nullptr || client_control_block == nullptr) {
        return ERR_VAL;
    }
    if (error != ERR_OK) {
        socket->state = INACTIVE;
        socket->pending_connection_reset = true;
        return error;
    }

    socket->connection_control_block = client_control_block;
    socket->socket_control_block = client_control_block;
    socket->rx_stream_buffer.clear();
    socket->state = CONNECTED;
    socket->pending_connection_reset = false;
    socket->connect_poll_ticks = 0;

    tcp_nagle_disable(client_control_block);
    tcp_arg(client_control_block, socket);
    tcp_recv(client_control_block, receive_callback);
    tcp_poll(client_control_block, poll_callback, 1);
    tcp_sent(client_control_block, send_callback);
    tcp_err(client_control_block, error_callback);
    if (socket->use_keep_alives) {
        config_keepalive(client_control_block, socket);
    }

    EthernetNode remote_node(socket->remote_ip, socket->remote_port);
    auto it = connecting_sockets.find(remote_node);
    if (it != connecting_sockets.end() && it->second == socket) {
        connecting_sockets.erase(it);
    }

    if (!socket->tx_packet_buffer.empty()) {
        socket->send();
    }
    return ERR_OK;
}

err_t Socket::receive_callback(
    void* arg,
    struct tcp_pcb* client_control_block,
    struct pbuf* packet_buffer,
    err_t error
) {
    Socket* socket = static_cast<Socket*>(arg);
    if (socket == nullptr) {
        if (packet_buffer != nullptr) {
            pbuf_free(packet_buffer);
        }
        return ERR_VAL;
    }
    socket->socket_control_block = client_control_block;
    if (packet_buffer == nullptr) { // FIN is received
        socket->state = CLOSING;
        return ERR_OK;
    }
    if (error != ERR_OK) {
        if (packet_buffer != nullptr) {
            pbuf_free(packet_buffer);
        }
        // Keep the socket alive on transient lwIP receive errors.
        return ERR_OK;
    }
    if (socket->state == CONNECTED) {
        socket->rx_packet_buffer.push(packet_buffer);
        socket->process_data();
        return ERR_OK;
    }
    pbuf_free(packet_buffer);
    return ERR_OK;
}

err_t Socket::poll_callback(void* arg, struct tcp_pcb* client_control_block) {
    Socket* socket = static_cast<Socket*>(arg);
    if (socket == nullptr) {
        tcp_abort(client_control_block);
        return ERR_ABRT;
    }
    socket->socket_control_block = client_control_block;

    if (!socket->tx_packet_buffer.empty()) {
        socket->send();
    }
    if (!socket->rx_packet_buffer.empty()) {
        socket->process_data();
    }
    if (socket->state == CLOSING) {
        socket->close();
    }
    return ERR_OK;
}

err_t Socket::send_callback(void* arg, struct tcp_pcb* client_control_block, uint16_t length) {
    (void)length;
    Socket* socket = static_cast<Socket*>(arg);
    if (socket == nullptr) {
        tcp_abort(client_control_block);
        return ERR_ABRT;
    }
    socket->socket_control_block = client_control_block;
    if (not socket->tx_packet_buffer.empty()) {
        socket->send();
    } else if (socket->state == CLOSING) {
        socket->close();
    }
    return ERR_OK;
}

void Socket::error_callback(void* arg, err_t error) {
    (void)error;
    Socket* socket = static_cast<Socket*>(arg);
    if (socket == nullptr) {
        return;
    }

    // tcp_err is called once the pcb is already freed by lwIP.
    socket->socket_control_block = nullptr;
    socket->connection_control_block = nullptr;
    socket->state = INACTIVE;
    socket->pending_connection_reset = true;
    socket->connect_poll_ticks = 0;
    socket->clear_packet_queues();
    socket->rx_stream_buffer.clear();
}

void Socket::connection_error_callback(void* arg, err_t error) {
    (void)error;
    Socket* socket = static_cast<Socket*>(arg);
    if (socket == nullptr) {
        return;
    }

    socket->connection_control_block = nullptr;
    socket->socket_control_block = nullptr;
    socket->state = INACTIVE;
    socket->pending_connection_reset = true;
    socket->connect_poll_ticks = 0;
    socket->rx_stream_buffer.clear();
}

err_t Socket::connection_poll_callback(void* arg, struct tcp_pcb* connection_control_block) {
    Socket* socket = static_cast<Socket*>(arg);
    if (socket == nullptr) {
        if (connection_control_block != nullptr) {
            tcp_abort(connection_control_block);
        }
        return ERR_ABRT;
    }

    socket->connection_control_block = connection_control_block;
    if (socket->pending_connection_reset) {
        socket->pending_connection_reset = false;
        socket->reset();
        return ERR_ABRT;
    }

    if (socket->state != CONNECTED && connection_control_block != nullptr &&
        connection_control_block->state == SYN_SENT) {
        socket->connect_poll_ticks++;
        if (socket->connect_poll_ticks >= 20) {
            socket->pending_connection_reset = true;
        }
    } else {
        socket->connect_poll_ticks = 0;
    }
    return ERR_OK;
}

void Socket::config_keepalive(tcp_pcb* control_block, Socket* socket) {
    control_block->so_options |= SOF_KEEPALIVE;
    control_block->keep_idle = socket->keepalive_config.inactivity_time_until_keepalive_ms;
    control_block->keep_intvl = socket->keepalive_config.space_between_tries_ms;
    control_block->keep_cnt = socket->keepalive_config.tries_until_disconnection;
}

#endif
