/*
 * SNTP.cpp
 *
 *  Created on: 21 feb. 2023
 *      Author: Ricardo
 */

#include "Communication/SNTP/SNTP.hpp"
#include <lwip/ip_addr.h>
#include "sntp_opts.h"
#include "sntp.h"
#include "IPV4/IPV4.hpp"
#include "Time/Time.hpp"

void SNTP::sntp_update(uint8_t address_head, uint8_t address_second, uint8_t address_third, uint8_t address_last){
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	ip4_addr_t* address;
	IP_ADDR4(address,address_head,address_second,address_third,address_last);
	sntp_setserver(0,address);
	sntp_init();
}

extern "C" void set_rtc(uint16_t counter, uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year){
	Time::set_RTC_data(counter, second, minute, hour, day, month, year);
}
