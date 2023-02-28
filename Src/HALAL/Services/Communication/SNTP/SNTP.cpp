/*
 * SNTP.cpp
 *
 *  Created on: 21 feb. 2023
 *      Author: Ricardo
 */

#include "Communication/SNTP/SNTP.hpp"

const ip4_addr *SNTP::sntp_server;

void SNTP::sntp_update(){
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setserver(0,SNTP::sntp_server);
	sntp_init();
}
