/*
 * HALAL.cpp
 *
 *  Created on: 5 ene. 2023
 *      Author: aleja
 */

#include "HALAL/HALAL.hpp"

void HALAL::start(TARGET target, string ip, string subnet_mask, string gateway, UART::Peripheral& printf_peripheral) {

#ifdef HAL_ETH_MODULE_ENABLED
	Ethernet::inscribe();
#endif

	HAL_Init();
	HALconfig::system_clock(target);
	HALconfig::peripheral_clock(target);

#ifdef HAL_GPIO_MODULE_ENABLED
	Pin::start();
#endif

#ifdef HAL_DMA_MODULE_ENABLED
	DMA::start();
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
	UART::set_up_printf(printf_peripheral);
#endif

#ifdef HAL_FDCAN_MODULE_ENABLED
	FDCAN::start();
#endif

#ifdef HAL_ETH_MODULE_ENABLED
	IPV4 ip4(ip);
	IPV4 subnet4(subnet_mask);
	IPV4 gateway4(gateway);
	Ethernet::start(ip4, subnet4, gateway4);
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
