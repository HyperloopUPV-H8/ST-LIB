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
		OPERATIONAL = 0,
		SLEEP = 1,
		FAULT = 2
	};

	enum TimeUnit : uint8_t {
		SECONDS = 0,
		MILLISECONDS = 1,
		MICROSECONDS = 2
	};

	static StateMachine state_machine;

	static void initialize(uint16_t transition_check_time_in_milliseconds = 500);

	static uint8_t add_cyclic_action(uint16_t milliseconds, function<void()> action, States state = OPERATIONAL, TimeUnit time_unit = MILLISECONDS);
	static void remove_cyclic_action(uint8_t id);

	static void add_transition(States old_state, States new_state, function<bool()> transition);
};
