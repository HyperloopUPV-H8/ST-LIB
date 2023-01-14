/*
 * HALAL.cpp
 *
 *  Created on: 5 ene. 2023
 *      Author: aleja
 */

#pragma once

#include "HALAL/HALAL.hpp"

void HALAL::start() {
	HAL_Init();
	HALconfig::system_clock();
	HALconfig::peripheral_clock();

	Pin::start();

#ifdef HAL_CORDIC_MODULE_ENABLED
	CORDIC_HandleTypeDef hcordic;
	hcordic.Instance = CORDIC;
	if (HAL_CORDIC_Init(&hcordic) != HAL_OK){}
#endif

#ifdef HAL_ADC_MODULE_ENABLED
	ADC::start();
#endif


#ifdef HAL_SPI_MODULE_ENABLED
	SPI::start();
#endif

#ifdef HAL_UART_MODULE_ENABLED
	UART::start();
#endif

#ifdef HAL_ETH_MODULE_ENABLED
	// Arreglar.
	//Ethernet::start(local_ip, subnet_mask, gateway);
#endif
#ifdef HAL_TIM_MODULE_ENABLED
	Encoder::start();
	TimerPeripheral::start();
	Time::start();
#endif

#ifdef HAL_EXTI_MODULE_ENABLED
	ExternalInterrupt::start();
#endif


}
