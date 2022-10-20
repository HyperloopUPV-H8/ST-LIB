/*
 * PWM_Module.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#include "PWM_Module.hpp"
#include "../C++Utilities/Utilities.hpp"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

unordered_map<Pin, TimerChannel, PWM::KeyHash, PWM::KeyEqual> pinTimerMap = {
		{PE2, {&htim1, 1}},
		{PE3, {&htim2, 1}},
		{PE4, {&htim3, 1}}
};

uint8_t PWM::ID_counter = 0;

uint8_t PWM::register_pwm(Pin& pin){
	if (pinTimerMap.find(pin) != pinTimerMap.end()) {
		PWM::serviceIDs[ID_counter] = pin;
		ID_counter++;
		return ID_counter-1;
	}

	return -1;
}

void PWM::unregister_pwm(uint8_t id){
	//PWM::serviceIDs.erase(id);
}

void PWM::turn_off_pwm(uint8_t id){
//	Pin pin = serviceIDs[id];
//	TimerChannel timerChannel = pinTimerMap[pin];
//
//	HAL_TIM_PWM_Stop(timerChannel.timer, timerChannel.channel);
}

void PWM::turn_on_pwm(uint8_t id){
//	Pin pin = serviceIDs[id];
//	TimerChannel timerChannel = pinTimerMap[pin];
//
//	HAL_TIM_PWM_Start(timerChannel.timer, timerChannel.channel);
}



void change_duty_cycle(uint8_t id, uint8_t duty_cycle) {
//
//	if (duty_cycle >= 0 && duty_cycle <= 100) {
//		Pin pin = PWM::serviceIDs[id];
//		TimerChannel timerChannel = pinTimerMap[pin];
//		__HAL_TIM_SET_COMPARE(timerChannel.timer, timerChannel.channel, duty_cycle);
//	}

}





