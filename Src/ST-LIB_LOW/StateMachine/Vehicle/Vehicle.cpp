/*
 * Vehicle.cpp
 *
 *  Created on: 13 dic. 2022
 *      Author: aleja
 */

#include "StateMachine/Vehicle/Vehicle.hpp"

void Vehicle::initialize() {
	state_machine = StateMachine(OFF);
	state_machine.add_state(ON);
	state_machine.add_state(SLEEP);

	optional<uint8_t> optional_id = Time::register_high_precision_alarm(500, []() {
		state_machine.check_transitions();
	});
	if (not optional_id) {
		//TODO: Error Handler
	}
}

void Vehicle::turn_off() {
	state_machine.force_change_state(OFF);
}

void Vehicle::turn_on() {
	state_machine.force_change_state(ON);
}

void Vehicle::to_sleep() {
	state_machine.force_change_state(SLEEP);
}

uint8_t Vehicle::add_cyclic_action(uint8_t milliseconds, function<void()> action) {
	optional<uint8_t> optional_id = Time::register_low_precision_alarm(milliseconds, [action](){
		if (state_machine.current_state == ON){
			action();
		}
	});
	if (not optional_id) {
		//TODO: Error Handler
	}

	return optional_id.value();
}

uint8_t Vehicle::add_cyclic_sleep_action(uint8_t milliseconds, function<void()> action) {
	optional<uint8_t> optional_id = Time::register_low_precision_alarm(milliseconds, [action](){
		if (state_machine.current_state == ON or state_machine.current_state == SLEEP){
			action();
		}
	});
	if (not optional_id) {
		//TODO: Error Handler
	}

	return optional_id.value();
}

void Vehicle::remove_cyclic_action(uint8_t id) {
	if (not Time::unregister_low_precision_alarm(id)) {
		//TODO: Error handler
	}
}

void Vehicle::add_transition(States old_state, States new_state, function<bool()> transition) {
	state_machine.add_transition(old_state, new_state, transition);
}
