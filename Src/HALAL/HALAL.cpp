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
	ADC::start();
	SPI::start();
	UART::start();
	Encoder::start();
	ExternalInterrupt::start();
	TimerPeripheral::start();
	Time::start();
}
