/*
 * HALAL.cpp
 *
 *  Created on: 5 ene. 2023
 *      Author: aleja
 */

#include "HALAL/HALAL.hpp"

void HALAL::start(IPV4 ip, IPV4 subnet_mask, IPV4 gateway, UART::Peripheral& printf_peripheral) {

#if defined USING_CMAKE && !defined STLIB_ETH
#else
	//Ethernet::inscribe();
#endif

	HAL_Init();
	HALconfig::system_clock();
	HALconfig::peripheral_clock();

#ifdef HAL_UART_MODULE_ENABLED
	UART::set_up_printf(printf_peripheral);
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
	Pin::start();
#endif

#ifdef HAL_DMA_MODULE_ENABLED
	DMA::start();
#endif

#ifdef HAL_CORDIC_MODULE_ENABLED
	CORDIC_HandleTypeDef hcordic;
	hcordic.Instance = CORDIC;
	if (HAL_CORDIC_Init(&hcordic) != HAL_OK){
		ErrorHandler("Unable to init CORDIC");
	}
#endif

#ifdef HAL_ADC_MODULE_ENABLED
	ADC::start();
#endif

#ifdef HAL_I2C_MODULE_ENABLED
	I2C::start();
#endif

#ifdef HAL_SPI_MODULE_ENABLED
	SPI::start();
#endif

#ifdef HAL_UART_MODULE_ENABLED
	UART::start();
#endif

#ifdef HAL_FDCAN_MODULE_ENABLED
	FDCAN::start();
#endif

#if defined USING_CMAKE && !defined STLIB_ETH
#else
	//Ethernet::start(ip, subnet_mask, gateway);
#endif

#ifdef HAL_TIM_MODULE_ENABLED
	Encoder::start();
	Time::start_rtc();
	SNTP::sntp_update();
	TimerPeripheral::start();
	Time::start();
#endif

#ifdef HAL_EXTI_MODULE_ENABLED
	ExternalInterrupt::start();
#endif


}
