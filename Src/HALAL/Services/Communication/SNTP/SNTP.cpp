/*
 * SNTP.cpp
 *
 *  Created on: 21 feb. 2023
 *      Author: Ricardo
 */

#include "Communication/SNTP/SNTP.hpp"

ip4_addr_t *SNTP::sntp_server;

void SNTP::sntp_update(uint8_t address_head, uint8_t address_second, uint8_t address_third, uint8_t address_last){
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	ip4_addr_t* address;
	IP_ADDR4(address,address_head,address_second,address_third,address_last);
	sntp_setserver(0,address);
	sntp_init();
}
