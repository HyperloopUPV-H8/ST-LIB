/*
 * SNTP.hpp
 *
 *  Created on: 21 feb. 2023
 *      Author: Ricardo
 */

#pragma once

#include "lwip/apps/sntp.h"
#include "HALAL/Models/IPV4/IPV4.hpp"
#include "C++Utilities/CppUtils.hpp"

class SNTP {
public:
    static constexpr const char* DEFAULT_SERVER_IP = "192.168.0.9";

    static void sntp_update(
        uint8_t address_head,
        uint8_t address_second,
        uint8_t address_third,
        uint8_t address_last
    );
    static void sntp_update(string ip);
    static void sntp_update();
};
