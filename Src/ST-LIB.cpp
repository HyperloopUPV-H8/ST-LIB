/**
  ******************************************************************************
  * @file     lib.c
  * @author   Auto-generated by STM32CubeIDE
  * @version  V1.0
  * @date     21/10/2022 11:26:50
  * @brief    Default under dev library file.
  ******************************************************************************
*/

#include "ST-LIB.hpp"

void STLIB::start(IPV4 ip, IPV4 subnet_mask, IPV4 gateway, UART::Peripheral& printf_peripheral) {
	HALAL::start(ip, subnet_mask, gateway, printf_peripheral);
	STLIB_LOW::start();
	STLIB_HIGH::start();
}

void STLIB::start(string ip, string subnet_mask, string gateaway,  UART::Peripheral& printf_peripheral){
	STLIB::start(IPV4(ip),IPV4(subnet_mask),IPV4(gateaway),printf_peripheral);
}

void STLIB::update() {
#if defined USING_CMAKE && !defined STLIB_ETH
#else
	Ethernet::update();
#endif
	ErrorHandlerModel::ErrorHandlerUpdate();
	Server::update_servers();
}
