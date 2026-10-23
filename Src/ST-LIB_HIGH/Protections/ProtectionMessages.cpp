/*
 * ProtectionMessages.cpp
 *
 *  Created on: Oct 22, 2026
 *      Author: Aleks
 */
#include "ST-LIB_HIGH/Protections/ProtectionMessages.hpp"
#include "HALAL/Services/Time/RTC.hpp"
#include "lwip/tcp.h"

namespace ST_LIB {

struct tcp_pcb* ProtectionMessagePacket::tpcb = nullptr;
bool ProtectionMessagePacket::is_connected = false;

static err_t tcp_client_connected_callback(void* arg, struct tcp_pcb* pcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    if (err == ERR_OK) {
        ProtectionMessagePacket::is_connected = true;
    }
    return ERR_OK;
}

ProtectionMessagePacket::Package ProtectionMessagePacket::make(uint16_t measurement_id, Protections::RuleKind prot, void* value, size_t valueSize)
{
    Package pack = {
        .packageSize = sizeof(Package_Data) + valueSize,
        .data = {}
    };

    // Bit packing of protection into 3 most significant bits of id (Look adjv3 format)
    pack.data.id = (measurement_id & 0x1FFF) | (static_cast<uint16_t>(prot) << 13);

    if (value != nullptr && valueSize > 0) {
        memcpy(pack.value, value, valueSize > 8 ? 8 : valueSize);
    }

#if defined(HAL_RTC_MODULE_ENABLED) && !defined(SIM_ON)
    RTCData rtc = Global_RTC::get_rtc_timestamp();
    pack.data.hours      = rtc.hour;
    pack.data.minutes    = rtc.minute;
    pack.data.seconds    = rtc.second;
    pack.data.subseconds = static_cast<uint8_t>((rtc.counter * 256) / (RTC_MAX_COUNTER + 1));
#else // if HAL_RTC was not enabled initialize all values to 0
    pack.data.hours = 0; pack.data.minutes = 0; pack.data.seconds = 0; pack.data.subseconds = 0;
#endif
    pack.data.reserved = 0;
    return pack;
}

void ProtectionMessagePacket::tcp_client_init()
{
    tpcb = tcp_new();
    if (tpcb != nullptr) {
        ip_addr_t destIPADDR;
        IP_ADDR4(&destIPADDR, 192, 168, 0, 9); // IP PC
        tcp_connect(tpcb, &destIPADDR, 31, tcp_client_connected_callback); // PORT
    }
}

bool ProtectionMessagePacket::tcp_client_send(const  ProtectionMessagePacket::Package& pack)
{
    if (tpcb != nullptr && is_connected) {
        err_t err = tcp_write(tpcb, &pack.data, pack.packageSize, TCP_WRITE_FLAG_COPY);
        if (err != ERR_OK) {
            return false;
        }
        err = tcp_output(tpcb);
        return (err == ERR_OK);
    }
    return false;
}

} // namespace ST_LIB
