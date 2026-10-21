#pragma once

#include "ST-LIB_HIGH/Protections/SampleSource.hpp"
#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "HALAL/Services/Time/RTC.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ST-LIB_HIGH/Protections/Protection.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionTypes.hpp"

// Forward declaration of LwIP packet buffer
struct pbuf;

/* value is an address / pointer to data */
#define MakeProtectionMessagePacket(value) \
    ST_LIB::ProtectionMessagePacket::make((value), sizeof(*(value)))

namespace ST_LIB {

struct ProtectionMessagePacket {
    // Header containing fault metadata and timestamp
    struct Package_Data {
        uint16_t id;
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        uint8_t reserved;
    };

    // Universal message packet
    struct Package {
        void* valuePtr;
        size_t packageSize;

        struct pbuf* pbuf;

        Package_Data data;
        // Maximum payload is 8 bytes (uint8, uint16, uint32, float, double, uint64)
        uint8_t value[8];
    };

    static Package make(void* value, size_t valueSize)
    {
        Package pack = {
            .valuePtr = value,
            .packageSize = sizeof(Package_Data) + valueSize,
            .pbuf = nullptr,
            .data = {},
            .value = {0},
        };

        // TODO [Step 1]: Fill pack.data with protection ID and RTC timestamp:
        //   auto rtc = Global_RTC::get_rtc_timestamp();
        //   pack.data.hours = rtc.hour;
        //   pack.data.minutes = rtc.minute;
        //   pack.data.seconds = rtc.second;

        // TODO [Step 2]: Copy the value bytes into pack.value array (up to 8 bytes):
        //   if (value != nullptr && valueSize > 0) {
        //       size_t len = (valueSize > 8) ? 8 : valueSize;
        //       memcpy(pack.value, value, len);
        //   }

        return pack;
    }

    static void send(Package pack)
    {
        // TODO [Step 3]: Call TCP transmission logic:
        //   Option A: If using LwIP raw API:
        //     tcp_write(pcb, &pack.data, pack.packageSize, TCP_WRITE_FLAG_COPY);
        //     tcp_output(pcb);
        //
        //   Option B: If using ST-LIB HALAL Socket:
        //     socket.send_order(...) / socket.send_packet(...)
    }

}; // struct ProtectionMessagePacket
}; // namespace ST_LIB