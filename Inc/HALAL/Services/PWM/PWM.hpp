/*
 * PWM_Module.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#pragma once

#include "../../Models/PinModel/Pin.hpp"
#include "../../../C++Utilities/CppUtils.hpp"


struct TimerChannel {
	TIM_HandleTypeDef* timer;
	uint32_t channel;
};

class PWM {
public:
	static map<uint8_t, Pin> serviceIDs;

	struct KeyHash {
		size_t operator()(const Pin& k) const {
			return hash<uint8_t>()(k.pin) ^
			(hash<uint8_t>()(*reinterpret_cast<uint64_t*>(k.port)) << 1);
		}
	};
	struct KeyEqual {
		bool operator()(const Pin lhs, const Pin rhs) const {
			return lhs.pin == rhs.pin && lhs.port == rhs.port;
	    }
	};
	static unordered_map<Pin, TimerChannel, KeyHash, KeyEqual> pinTimerMap;
	static forward_list<uint8_t> IDmanager;

	static optional<uint8_t> register_pwm(Pin pin);
	static void unregister_pwm(uint8_t id);
	static void turn_on_pwm(uint8_t id);
	static void turn_off_pwm(uint8_t id);
	static void change_duty_cycle(uint8_t id, uint8_t duty_cicle);
};
