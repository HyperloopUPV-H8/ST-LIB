/*
 * PWM_Module.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: alejandro
 */

#include "PWMservice/PWMservice.hpp"


forward_list<uint8_t> PWMservice::id_manager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
map<uint8_t, PWMservice::Instance> PWMservice::active_instances = {};

PWMservice::TimerInitData::TimerInitData(TIM_TypeDef* timer, GPIO_TypeDef* gpio_port, uint32_t prescaler, uint32_t period) :
		timer(timer), gpio_port(gpio_port), prescaler(prescaler), period(period), channels({}) {}

PWMservice::TimerPeripheral::TimerPeripheral(TIM_HandleTypeDef* handle, TimerInitData init_data) :
		handle(handle), init_data(init_data) {}
bool PWMservice::TimerPeripheral::is_registered() {
	return init_data.channels.size();
}

PWMservice::Instance::Instance(TimerPeripheral* peripheral, uint32_t channel, PWMservice::Mode mode) :
		peripheral(peripheral), channel(channel), mode(mode) {}

optional<uint8_t> PWMservice::inscribe(Pin& pin){
	if (not available_instances.contains(pin)) {
		return nullopt; //TODO: error handlerr
	}

	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = id_manager.front();
	active_instances[id] = available_instances[pin];
	id_manager.pop_front();

	TimerInitData& init_data = active_instances[id].peripheral->init_data;
	init_data.channels.push_back(active_instances[id].channel);
	init_data.gpio_pins |= pin.gpio_pin;
	return id;
}

optional<uint8_t> PWMservice::inscribe_negated(Pin& pin) {
	if (not available_instances_negated.contains(pin)) {
		return nullopt; //TODO: error handlerr
	} 	
	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = id_manager.front();
	active_instances[id] = available_instances_negated[pin];
	id_manager.pop_front();

	TimerInitData& init_data = active_instances[id].peripheral->init_data;
	init_data.channels.push_back(active_instances[id].channel);
 	return id;
}

optional<uint8_t> PWMservice::inscribe_dual(Pin& pin, Pin& pin_negated){
	if (not available_instances_dual.contains({pin, pin_negated})) {
		return nullopt; //TODO: error handlerr
	} 	
	Pin::inscribe(pin, ALTERNATIVE);
	Pin::inscribe(pin_negated, ALTERNATIVE);
	uint8_t id = PWMservice::id_manager.front();
	active_instances[id] = available_instances_dual[{pin, pin_negated}];
	id_manager.pop_front();

	TimerInitData& init_data = active_instances[id].peripheral->init_data;
	init_data.channels.push_back(active_instances[id].channel);
	return id;
}

void PWMservice::start() {
	for(TimerPeripheral peripheral : timer_peripherals) {
		if (peripheral.is_registered()) {
			PWMservice::init(peripheral);
		}
	}
}
void PWMservice::turn_on(uint8_t id) {
	if (not instance_exists(id)) {
		//TODO: error handlerr
	}

	Instance& instance = get_instance(id);

	if (instance.mode == NORMAL) {
		HAL_TIM_PWM_Start(instance.peripheral->handle, instance.channel);
	}

	else if (instance.mode == NEGATED) {
		HAL_TIMEx_PWMN_Start(instance.peripheral->handle, instance.channel);
	}

	else if(instance.mode == DUAL) {
		HAL_TIMEx_PWMN_Start(instance.peripheral->handle, instance.channel);
	}
}

void PWMservice::turn_off(uint8_t id){
	if (not instance_exists(id)) {
		//TODO: Error handler
	}

	Instance& instance = get_instance(id);
	if (instance.mode == NORMAL) {
		HAL_TIM_PWM_Stop(instance.peripheral->handle, instance.channel);
	}

	else if (instance.mode == NEGATED) {
		HAL_TIMEx_PWMN_Stop(instance.peripheral->handle, instance.channel);
	}

	else if (instance.mode == DUAL) {
		HAL_TIM_PWM_Stop(instance.peripheral->handle, instance.channel);
		HAL_TIMEx_PWMN_Stop(instance.peripheral->handle, instance.channel);
	}

	else {
		//TODO: Error handler
	}
}

void PWMservice::set_duty_cycle(uint8_t id, uint8_t duty_cycle) {
	if (not (duty_cycle >= 0 && duty_cycle <= 100)) {
		//TODO: error handlerr
		return;
	}
	if (not instance_exists(id)) {
		//TODO: error handlerr
		return;
	}

	Instance& instance = get_instance(id);
	uint16_t raw_duty = round(__HAL_TIM_GET_AUTORELOAD(instance.peripheral->handle) / 100.0 * duty_cycle);
	__HAL_TIM_SET_COMPARE(instance.peripheral->handle, instance.channel, raw_duty);
}

void PWMservice::init(TimerPeripheral& peripheral) {
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	TIM_HandleTypeDef& tim_handle = *peripheral.handle;
	TimerInitData init_data = peripheral.init_data;


	tim_handle.Instance = init_data.timer;
	tim_handle.Init.Prescaler = init_data.prescaler;
	tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim_handle.Init.Period = init_data.period;
	tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim_handle.Init.RepetitionCounter = 0;
	tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&tim_handle) != HAL_OK) {
		//TODO: error handler
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&tim_handle, &sMasterConfig) != HAL_OK) {
		//TODO: error handler
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	for (uint32_t channel : init_data.channels) {
		if (HAL_TIM_PWM_ConfigChannel(&tim_handle, &sConfigOC, channel) != HAL_OK) {
			//TODO: Error handler
		}
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&tim_handle, &sBreakDeadTimeConfig) != HAL_OK) {
		//TODO: Error Handler
	}

	GPIO_InitTypeDef GPIO_InitStruct = {0};


	GPIO_InitStruct.Pin = init_data.gpio_pins;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(init_data.gpio_port, &GPIO_InitStruct);
}

bool PWMservice::instance_exists(uint8_t id) {
	if (PWMservice::active_instances.contains(id)) {
		return true;
	}

	return false;
}

PWMservice::Instance& PWMservice::get_instance(uint8_t id) {
	return PWMservice::active_instances[id];
}


