/*
 * PWM_Module.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#ifndef HALAL_PWM_MODULE_PWM_MODULE_HPP_
#define HALAL_PWM_MODULE_PWM_MODULE_HPP_

#include "../../Models/PinModel/Pin.hpp"
#include "../Inc/C++Utilities/CppUtils.hpp"


struct TimerChannel {
	TIM_HandleTypeDef* timer;
	uint32_t channel;
};

class PWM {
public:
	static map<uint8_t, Pin> serviceIDs;

	struct KeyHash {
	         std::size_t operator()(const Pin& k) const
	         {
	             return std::hash<std::uint8_t>()(k.pin) ^
	                    (std::hash<std::uint8_t>()(*reinterpret_cast<uint64_t*>(k.port)) << 1);
	         }
	        };
	        struct KeyEqual {
	         bool operator()(const Pin& lhs, const Pin& rhs) const
	         {
	            return lhs.pin == rhs.pin && lhs.port == rhs.port;
	         }
	        };
	static unordered_map<Pin, TimerChannel, KeyHash, KeyEqual> pinTimerMap;
	static forward_list<uint8_t> IDmanager;

	static uint8_t register_pwm(Pin& pin);
	static void unregister_pwm(uint8_t id);
	static void turn_on_pwm(uint8_t id);
	static void turn_off_pwm(uint8_t id);
	static void change_duty_cicle(uint8_t id, uint8_t duty_cicle);
};




#endif /* HALAL_PWM_MODULE_PWM_MODULE_HPP_ */
