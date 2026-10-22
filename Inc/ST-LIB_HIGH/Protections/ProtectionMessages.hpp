#pragma once
/*
 * ProtectionMessages.hpp
 *      "Hello there"
 *  Created on: Oct 22, 2026
 *      Author: Aleks
 */
#include "ST-LIB_HIGH/Protections/SampleSource.hpp"
#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "HALAL/Services/Time/RTC.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ST-LIB_HIGH/Protections/Protection.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionTypes.hpp"
#include "lwip/tcp.h"

namespace ST_LIB {
struct ProtectionMessagePacket {

    struct Package_Data {
      
        uint16_t id;

        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        uint8_t reserved;  
    };


    struct Package {
        size_t packageSize;
        Package_Data data;
        // Maximum is 8 bytes, not all will be sent always
        uint8_t value[8];
    };
    // According to AI and Google it makes no difference use Macros to calculate Size or Template in this case 
    template <typename T>
    static Package make(uint16_t measurement_id, Protections::RuleKind prot, T* value)
    {
        static_assert(sizeof(T) <= 8, "Value size must be <= 8 bytes");
        return make(measurement_id, prot, static_cast<void*>(value), sizeof(T));
    }
    static Package make(uint16_t measurement_id, Protections::RuleKind prot, void* value, size_t valueSize)
    {
        Package pack = {
            .packageSize = sizeof(Package_Data) + valueSize,
            .data = {}

        };
        pack.data.id = (measurement_id & 0x1FFF) | static_cast<uint16_t> (static_cast<uint8_t>(prot) << 13);
        if (value != nullptr && valueSize > 0) {
            memcpy(pack.value, value, valueSize);
        }
        #if defined(HAL_RTC_MODULE_ENABLED) && !defined(SIM_ON)
            RTCData rtc = Global_RTC::get_rtc_timestamp();
            pack.data.hours   = rtc.hour;
            pack.data.minutes = rtc.minute;
            pack.data.seconds = rtc.second;
        #else
            pack.data.hours = 0; pack.data.minutes = 0; pack.data.seconds = 0;
        #endif
        pack.data.reserved = 0;
        return pack;
    }
    //TCP STUFF starts here
    static inline struct tcp_pcb* tpcb;
    static inline bool is_connected = false;
    //This code probably can be refactored
    static err_t tcp_client_connected(void* arg,  tcp_pcb* tcpb, err_t err)
    {
        LWIP_UNUSED_ARG(tcpb);
        LWIP_UNUSED_ARG(arg);
        if (err == ERR_OK) {
            is_connected = true;
        }
        return ERR_OK;
    }
    static void tcp_client_init()
    {
        tpcb = tcp_new();
        if (tpcb != NULL) {
            ip_addr_t destIPADDR;
            IP_ADDR4(&destIPADDR, 192, 168, 0, 100); //IP
            tcp_connect(tpcb, &destIPADDR, 31, tcp_client_connected); //PORT
        }
    }
    static void tcp_client_send(const Package& pack) //add some timer??
    {
        if (tpcb != nullptr && is_connected){
            //pack.pbuf = pbuf_alloc(PBUF_TRANSPORT, pack.packageSize, PBUF_RAM); im suprized we dont need it
            tcp_write(tpcb, &pack.data, pack.packageSize, TCP_WRITE_FLAG_COPY);
            
            tcp_output(tpcb);
        }
    }

}; // struct ProtectionMessagePacket
}; // namespace ST_LIB
