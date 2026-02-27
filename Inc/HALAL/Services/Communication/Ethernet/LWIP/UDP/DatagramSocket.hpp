#pragma once
#include "HALAL/Models/Packets/Packet.hpp"
#include "HALAL/Services/Communication/Ethernet/LWIP/Ethernet.hpp"
#include "HALAL/Services/Communication/Ethernet/LWIP/EthernetNode.hpp"

#ifdef HAL_ETH_MODULE_ENABLED
#define PBUF_POOL_MEMORY_DESC_POSITION 8

class DatagramSocket {
public:
    struct udp_pcb* udp_control_block;

    IPV4 local_ip;
    uint32_t local_port;
    IPV4 remote_ip;
    uint32_t remote_port;
    bool is_disconnected = true;
    DatagramSocket();
    DatagramSocket(DatagramSocket&& other);
    DatagramSocket(IPV4 local_ip, uint32_t local_port, IPV4 remote_ip, uint32_t remote_port);
    DatagramSocket(EthernetNode local_node, EthernetNode remote_node);
    ~DatagramSocket();

    void operator=(DatagramSocket&&);

    void reconnect();

    static void receive_callback(
        void* args,
        struct udp_pcb* udp_control_block,
        struct pbuf* packet_buffer,
        const ip_addr_t* remote_address,
        u16_t port
    );
    bool send_packet(Packet& packet) {
        if (is_disconnected || udp_control_block == nullptr) {
            return false;
        }
        uint8_t* packet_buffer = packet.build();
        if (packet_buffer == nullptr || packet.size == 0) {
            return false;
        }

        struct pbuf* tx_buffer = pbuf_alloc(PBUF_TRANSPORT, packet.size, PBUF_RAM);
        if (tx_buffer == nullptr) {
            return false;
        }
        if (pbuf_take(tx_buffer, packet_buffer, packet.size) != ERR_OK) {
            pbuf_free(tx_buffer);
            return false;
        }
        err_t send_error = udp_send(udp_control_block, tx_buffer);
        pbuf_free(tx_buffer);

        return send_error == ERR_OK;
    }

    void close();
};

#endif
