/*
 * PWM_Module.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: alejandro 
 */

#pragma once

#include "PinModel/Pin.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

#define H723_TIMERS 24

class PWM {
public:

	enum Mode {
		normal = 0,
		negated = 1,
		dual = 2
	};
	struct TimerInitData {
	public:
		TIM_TypeDef* timer;
		uint32_t prescaler;
		uint32_t period;
		vector<uint32_t> channels;
		TimerInitData() = default;
		TimerInitData(TIM_TypeDef* timer, uint32_t prescaler, uint32_t period);
	};

	class TimerPeripheral {
	public:
		TIM_HandleTypeDef* handle;
		TimerInitData init_data;

		TimerPeripheral() = default;
		TimerPeripheral(TIM_HandleTypeDef* handle, TimerInitData init_data);

		bool is_registered();
	};

	class Instance {
	public:
		TimerPeripheral* peripheral;
		uint32_t channel;
		Mode mode;

		Instance() = default;
		Instance(TimerPeripheral* peripheral, uint32_t channel, Mode mode);

	};
	static map<uint8_t, Instance> active_instances;
	static map<Pin, Instance> available_instances;
	static map<Pin, Instance> available_instances_negated;
	static map<pair<Pin, Pin>, Instance> available_instances_dual;
	static TimerPeripheral timer_peripherals[H723_TIMERS];

	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> inscribe(Pin& pin);
	static optional<uint8_t> inscribe_negated(Pin& pin);
	static optional<uint8_t> inscribe_dual(Pin& pin, Pin& pin_negated);
	static void start();
	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static void set_duty_cycle(uint8_t id, uint8_t duty_cicle);
private:
	static void init(TimerPeripheral& peripheral);
	static bool instance_exists(uint8_t id);
	static Instance& get_instance(uint8_t id);
};

#endif
