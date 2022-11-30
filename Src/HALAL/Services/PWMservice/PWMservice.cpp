/*
 * PWM_Module.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: alejandro
 */

#include "PWMservice/PWMservice.hpp"

forward_list<uint8_t> PWMservice::id_manager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
map<uint8_t,Pin> PWMservice::service_ids = {};
map<uint8_t,Pin> PWMservice::service_ids_negated = {};
map<uint8_t,pair<Pin, Pin>> PWMservice::service_ids_dual = {};

optional<uint8_t> PWMservice::inscribe(Pin& pin){
	if (not PWMservice::pin_timer_map.contains(pin)) {
		return nullopt; //TODO: error handler
	}

	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = PWMservice::id_manager.front();
	PWMservice::service_ids[id] = pin;
	PWMservice::id_manager.pop_front();
	return id;
}

optional<uint8_t> PWMservice::inscribe_negated(Pin& pin) {
	if (not pin_timer_map_negated.contains(pin)) {
		return nullopt; //TODO: error handler
	} 	
	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = PWMservice::id_manager.front();
	PWMservice::service_ids_negated[id] = pin;
	PWMservice::id_manager.pop_front();
 	return id;
}

optional<uint8_t> PWMservice::inscribe_dual(Pin& pin, Pin& pin_negated){
	if (not PWMservice::pin_timer_map_dual.contains({pin, pin_negated})) {
		return nullopt; //TODO: error handler
	} 	
	Pin::inscribe(pin, ALTERNATIVE);
	Pin::inscribe(pin_negated, ALTERNATIVE);
	uint8_t id = PWMservice::id_manager.front();
	PWMservice::service_ids_dual[id] = {pin, pin_negated};
	PWMservice::id_manager.pop_front();
	return id;
}

void PWMservice::turn_on(uint8_t id){
	if (PWMservice::service_ids.contains(id)) {
		Pin pin = PWMservice::service_ids[id];
		TimerChannel tim_ch = PWMservice::pin_timer_map[pin];
		HAL_TIM_PWM_Start(tim_ch.timer, tim_ch.channel);
	}

	else if(PWMservice::service_ids_negated.contains(id)) {
		Pin pin = PWMservice::service_ids_negated[id];
		TimerChannel tim_ch = PWMservice::pin_timer_map_negated[pin];
		HAL_TIMEx_PWMN_Start(tim_ch.timer, tim_ch.channel);
	}

	else if(PWMservice::service_ids_dual.contains(id)) {
		pair<Pin, Pin> pin_pair = PWMservice::service_ids_dual[id];
		TimerChannel tim_ch = PWMservice::pin_timer_map_dual[pin_pair];
		HAL_TIM_PWM_Start(tim_ch.timer, tim_ch.channel);
		HAL_TIMEx_PWMN_Start(tim_ch.timer, tim_ch.channel);
	}

	else {
		//TODO: error handler
	}
}

void PWMservice::turn_off(uint8_t id){
	if (PWMservice::service_ids.contains(id)) {
		Pin pin = PWMservice::service_ids[id];
		TimerChannel tim_ch = PWMservice::pin_timer_map[pin];
		HAL_TIM_PWM_Stop(tim_ch.timer, tim_ch.channel);
	}

	else if (PWMservice::service_ids_negated.contains(id)) {
		Pin pin = PWMservice::service_ids_negated[id];
		TimerChannel tim_ch = PWMservice::pin_timer_map_negated[pin];
		HAL_TIMEx_PWMN_Stop(tim_ch.timer, tim_ch.channel);
	}

	else if (PWMservice::service_ids_dual.contains(id)) {
		pair<Pin, Pin> pin_pair = PWMservice::service_ids_dual[id];
		TimerChannel tim_ch = PWMservice::pin_timer_map_dual[pin_pair];
		HAL_TIM_PWM_Stop(tim_ch.timer, tim_ch.channel);
		HAL_TIMEx_PWMN_Stop(tim_ch.timer, tim_ch.channel);
	}

	else {
		//TODO: Error handler
	}
}

void PWMservice::set_duty_cycle(uint8_t id, uint8_t duty_cycle) {
	if (not (duty_cycle >= 0 && duty_cycle <= 100)) {
		//TODO: error handler
		return;
	}

	TimerChannel tim_ch;
	if (PWMservice::service_ids.contains(id)) {
		Pin pin = PWMservice::service_ids[id];
		tim_ch = PWMservice::pin_timer_map[pin];
	}

	else if (PWMservice::service_ids_negated.contains(id)) {
		Pin pin = PWMservice::service_ids_negated[id];
		tim_ch = PWMservice::pin_timer_map_negated[pin];
	}

	else if (PWMservice::service_ids_dual.contains(id)) {
		pair<Pin, Pin> pin_pair = PWMservice::service_ids_dual[id];
		tim_ch = PWMservice::pin_timer_map_dual[pin_pair];
	}
	else {
		//TODO: error handler
		return;
	}
	uint16_t raw_duty = round(__HAL_TIM_GET_AUTORELOAD(tim_ch.timer) / 100.0 * duty_cycle);
	__HAL_TIM_SET_COMPARE(tim_ch.timer, tim_ch.channel, raw_duty);
}





