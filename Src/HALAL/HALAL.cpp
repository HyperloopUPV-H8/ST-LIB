/*
 * HALAL.cpp
 *
 *  Created on: 5 ene. 2023
 *      Author: aleja
 */

#include "HALAL/HALAL.hpp"

void HALAL::start() {
	#ifdef HAL_ETH_MODULE_ENABLEDD
		Ethernet::inscribe();
	#endif

	HAL_Init();
	HALconfig::system_clock();
	HALconfig::peripheral_clock();

#ifdef HAL_GPIO_MODULE_ENABLED
	Pin::start();
#endif

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

#ifdef HAL_ETH_MODULE_ENABLEDD
	Ethernet::start();
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
