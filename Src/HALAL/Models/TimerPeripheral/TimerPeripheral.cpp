/*
 * TimerPeripheral.cpp
 *
 *  Created on: 3 dic. 2022
 *      Author: aleja
 */

#include "TimerPeripheral/TimerPeripheral.hpp"

vector<reference_wrapper<TimerPeripheral>> TimerPeripheral::timers;

TimerPeripheral::InitData::InitData(
		TIM_TypeDef* timer, uint32_t prescaler, uint32_t period, uint32_t deadtime) :
		timer(timer),
		prescaler(prescaler),
		period(period),
		deadtime(deadtime),
		pwm_channels({}),
		input_capture_channels({}) {}

TimerPeripheral::TimerPeripheral(
		TIM_HandleTypeDef* handle, InitData init_data) :
		handle(handle),
		init_data(init_data) {}

void TimerPeripheral::init() {
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		TIM_IC_InitTypeDef sConfigIC = {0};
		TIM_OC_InitTypeDef sConfigOC = {0};
		TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

		handle->Instance = init_data.timer;
		handle->Init.Prescaler = init_data.prescaler;
		handle->Init.CounterMode = TIM_COUNTERMODE_UP;
		handle->Init.Period = init_data.period;
		handle->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		handle->Init.RepetitionCounter = 0;
		handle->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_PWM_Init(handle) != HAL_OK) {
			//TODO: error handler
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(handle, &sMasterConfig) != HAL_OK) {
			//TODO: error handler
		}
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 0;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

		for (uint32_t channel : init_data.pwm_channels) {
			if (HAL_TIM_PWM_ConfigChannel(handle, &sConfigOC, channel) != HAL_OK) {
				//TODO: Error handler
			}
		}

		sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
		sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
		sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
		sBreakDeadTimeConfig.DeadTime = init_data.deadtime;
		sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
		sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
		sBreakDeadTimeConfig.BreakFilter = 0;
		sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
		sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
		sBreakDeadTimeConfig.Break2Filter = 0;
		sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
		if (HAL_TIMEx_ConfigBreakDeadTime(handle, &sBreakDeadTimeConfig) != HAL_OK) {
			//TODO: Error Handler
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 0;
		for (pair<uint32_t, uint32_t> channels_rising_falling : init_data.input_capture_channels) {
			sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
			if (HAL_TIM_IC_ConfigChannel(handle, &sConfigIC, channels_rising_falling.first) != HAL_OK) {
				//TODO: Error handler
			}

			sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
			if (HAL_TIM_IC_ConfigChannel(handle, &sConfigIC, channels_rising_falling.second) != HAL_OK) {
				//TODO: Error handler
			}
		}
}

void TimerPeripheral::start() {
	for (TimerPeripheral timer : timers) {
		if (timer.is_registered()) {
			timer.init();
		}
	}
}

bool TimerPeripheral::is_registered() {
	return init_data.pwm_channels.size() + init_data.input_capture_channels.size();
}
