/*
 * TimerPeripheral.cpp
 *
 *  Created on: 3 dic. 2022
 *      Author: aleja
 */

#include "TimerPeripheral/TimerPeripheral.hpp"

TimerPeripheral::InitData::InitData(
		TIM_TypeDef* timer, uint32_t prescaler, uint32_t period, uint32_t deadtime) :
		timer(timer),
		prescaler(prescaler),
		period(period),
		deadtime(deadtime),
		channels({}) {}

TimerPeripheral::TimerPeripheral(
		TIM_HandleTypeDef* handle, InitData init_data) :
		handle(handle),
		init_data(init_data) {}

bool TimerPeripheral::is_registered() {
	return init_data.channels.size();
}
