/*
 * PWM_Module.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#include "../Inc/HALAL/Services/PWM/PWM.hpp"

extern TIM_HandleTypeDef htim1;

forward_list<uint8_t> PWM::IDmanager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254};

//TimerChannel tim1ch1((uint32_t)24, TIM_CHANNEL_1);
map<Pin, TimerChannel> PWM::pinTimerMap = {
		{PE9, {&htim1, TIM_CHANNEL_1}}
};
map<uint8_t,Pin> PWM::serviceIDs = {};


optional<uint8_t> PWM::register_pwm(Pin& pin){
	if (PWM::pinTimerMap.find(pin) == PWM::pinTimerMap.end()) { return {}; }
	Pin::register_pin(pin, ALTERNATIVE);
	uint8_t id = PWM::IDmanager.front();
	PWM::serviceIDs[id] = pin;
	PWM::IDmanager.pop_front();
	return id;
}

void PWM::unregister_pwm(uint8_t id){
	Pin::unregister_pin(PWM::serviceIDs[id]);
	PWM::serviceIDs.erase(id);
	PWM::IDmanager.push_front(id);
}

void PWM::turn_off_pwm(uint8_t id){
	Pin pin = PWM::serviceIDs[id];
	TimerChannel timerChannel = PWM::pinTimerMap[pin];
	HAL_TIM_PWM_Stop(timerChannel.timer, timerChannel.channel);
}

void PWM::turn_on_pwm(uint8_t id){
	Pin pin = PWM::serviceIDs[id];
	TimerChannel timerChannel = PWM::pinTimerMap[pin];
	HAL_TIM_PWM_Start(timerChannel.timer, timerChannel.channel);
}



void PWM::change_duty_cycle(uint8_t id, uint8_t duty_cycle) {
	if (duty_cycle >= 0 && duty_cycle <= 100) {
		Pin pin = PWM::serviceIDs[id];
		TimerChannel timerChannel = PWM::pinTimerMap[pin];
		uint8_t raw_duty = timerChannel.timer -> Init.Period / 100 * duty_cycle;
		__HAL_TIM_SET_COMPARE(timerChannel.timer, timerChannel.channel, raw_duty);
	}
}





