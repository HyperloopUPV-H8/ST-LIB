#pragma once

#include "ST-LIB_HIGH/Protections/SampleSource.hpp"
#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "HALAL/Services/Time/RTC.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ST-LIB_HIGH/Protections/Protection.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionTypes.hpp"

namespace ST_LIB {
#pragma pack(push, 1)
template <typename T> struct ProtectionMessagePacket {
    // Comprobar si es de 1 4 8 16 32 o 64 bytes(permitido)
    static_assert(sizeof(T) <= 64);
    uint16_t id;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t reserved;
    T value;
    ProtectionMessagePacket(uint16_t measurement_id, Protections::RuleKind protection, T val)
        : id((static_cast<uint16_t>(static_cast<uint16_t>(protection) << 13)) |
             (measurement_id & 0X1FFF)),
          hours(Global_RTC::get_rtc_timestamp().hour),
          minutes(Global_RTC::get_rtc_timestamp().minute),
          seconds(Global_RTC::get_rtc_timestamp().second), reserved(0), value(val) {}
    // Protection detection
    // TCP
}; // struct ProtectionMessagePacket
#pragma pack(pop) 
}; // namespace ST_LIB
