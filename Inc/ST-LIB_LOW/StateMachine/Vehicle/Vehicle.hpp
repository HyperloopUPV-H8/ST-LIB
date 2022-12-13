/*
 * Vehicle.hpp
 *
 *  Created on: 13 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "StateMachine/StateMachine.hpp"
#include "Time/Time.hpp"

class Vehicle {
public:
	enum States : uint8_t {
		OFF = 0,
		ON = 1,
		SLEEP = 2
	};

	static StateMachine state_machine;

	static void initialize();
	static void turn_on();
	static void turn_off();
	static void to_sleep();

	static uint8_t add_cyclic_action(uint16_t milliseconds, function<void()> action);
	static uint8_t add_cyclic_sleep_action(uint8_t milliseconds, function<void()> action);
	static void remove_cyclic_action(uint8_t id);

	static void add_transition(States old_state, States new_state, function<bool()> transition);
};
