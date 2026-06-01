/*
 * HALAL.cpp
 *
 *  Created on: 5 ene. 2023
 *      Author: aleja
 */

#include "HALAL/HALAL.hpp"
#include "stm32h7xx_hal_eth.h"

#ifndef STLIB_ETH
ETH_HandleTypeDef heth;
void HAL_ETH_IRQHandler(ETH_HandleTypeDef* heth_arg) { (void)heth_arg; }
#endif // STLIB_ETH
