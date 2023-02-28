/*
 * TimerPeripheral.cpp
 *
 *  Created on: 3 dic. 2022
 *      Author: aleja
 */

#include "TimerPeripheral/TimerPeripheral.hpp"

map<TIM_HandleTypeDef*, TIM_TypeDef*> TimerPeripheral::handle_to_timer= {
		{&htim1, TIM1},
		{&htim2, TIM2},
		{&htim3, TIM3},
		{&htim4, TIM4},
		{&htim5, TIM5},
		{&htim6, TIM6},
		{&htim7, TIM7},
		{&htim8, TIM8},
		{&htim12, TIM12},
		{&htim13, TIM13},
		{&htim14, TIM14},
		{&htim15, TIM15},
		{&htim16, TIM16},
		{&htim17, TIM17},
		{&htim23, TIM23},
		{&htim24, TIM24},
};


TimerPeripheral::InitData::InitData(
		bool is_base, uint32_t prescaler, uint32_t period, uint32_t deadtime) :
		timer(timer),
		prescaler(prescaler),
		period(period),
		deadtime(deadtime),
		is_base(is_base)
		{}

TimerPeripheral::TimerPeripheral(
		TIM_HandleTypeDef* handle, InitData& init_data, string name) :
		handle(handle),
		init_data(init_data),
		name(name) {}

void TimerPeripheral::init() {
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		TIM_IC_InitTypeDef sConfigIC = {0};
		TIM_OC_InitTypeDef sConfigOC = {0};
		TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

		handle->Instance = handle_to_timer[handle];
		handle->Init.Prescaler = init_data.prescaler;
		handle->Init.CounterMode = TIM_COUNTERMODE_UP;
		handle->Init.Period = init_data.period;
		handle->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		handle->Init.RepetitionCounter = 0;
		handle->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

		if (init_data.is_base) {
			if (HAL_TIM_Base_Init(handle) != HAL_OK){
				ErrorHandler("Unable to init base timer on %d", name.c_str());
			}
		}

		if (!init_data.input_capture_channels.empty()) {
			if (HAL_TIM_IC_Init(handle) != HAL_OK)
			{
				ErrorHandler("Unable to init input capture on %d", name.c_str());
			}
		}

		if (!init_data.pwm_channels.empty()) {
			if (HAL_TIM_PWM_Init(handle) != HAL_OK) {
				ErrorHandler("Unable to init PWM on %d", name.c_str());
			}
		}

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(handle, &sMasterConfig) != HAL_OK) {
			ErrorHandler("Unable to configure master synchronization on %d", name.c_str());
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 0;
		for (pair<uint32_t, uint32_t> channels_rising_falling : init_data.input_capture_channels) {
			sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
			if (HAL_TIM_IC_ConfigChannel(handle, &sConfigIC, channels_rising_falling.first) != HAL_OK) {
				ErrorHandler("Unable to configure a IC channel on %d", name.c_str());
			}

			sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
			sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
			if (HAL_TIM_IC_ConfigChannel(handle, &sConfigIC, channels_rising_falling.second) != HAL_OK) {
				ErrorHandler("Unable to configure a IC channel on %d", name.c_str());
			}
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
				ErrorHandler("Unable to configure a PWM channel on %d", name.c_str());
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
			ErrorHandler("Unable to configure break dead time on %d", name.c_str());
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

uint32_t TimerPeripheral::get_prescaler() {
	return handle->Instance->PSC;
}

uint32_t TimerPeripheral::get_period() {
	return handle->Instance->ARR;
}
