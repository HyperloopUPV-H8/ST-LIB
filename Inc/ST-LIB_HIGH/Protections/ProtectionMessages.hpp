#pragma once
/*
 * ProtectionMessages.hpp
 *      "Hello there"
 *  Created on: Oct 22, 2026
 *      Author: Aleks
 */
#include "ST-LIB_HIGH/Protections/SampleSource.hpp"
#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ST-LIB_HIGH/Protections/Protection.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionTypes.hpp"
#include <cstdint>
#include <cstddef>
#include <cstring>

// Forward declaration of LwIP tcp_pcb
struct tcp_pcb;

namespace ST_LIB {

#pragma pack(push, 1)
struct ProtectionMessagePacket {

    struct Package_Data {
        uint16_t id;
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        uint8_t subseconds;
        uint16_t reserved;  // Just to not forget about it exists (align / array index in ADJv3)
    };

    struct Package {
        size_t packageSize;
        Package_Data data;
        // Maximum is 8 bytes, not all will be sent always
        uint8_t value[8];
    };
#pragma pack(pop)

    template <typename T>
    static Package make(uint16_t measurement_id, Protections::RuleKind prot, T* value)
    {
        static_assert(sizeof(T) <= 8, "Value size must be <= 8 bytes");
        return make(measurement_id, prot, static_cast<void*>(value), sizeof(T));
    }

    static Package make(uint16_t measurement_id, Protections::RuleKind prot, void* value, size_t valueSize);
    static void tcp_client_init();
    static bool tcp_client_send(const Package& pack);

    static struct tcp_pcb* tpcb;
    static bool is_connected;
};

} // namespace ST_LIB
