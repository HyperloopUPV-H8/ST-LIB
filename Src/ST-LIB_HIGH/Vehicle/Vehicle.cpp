/*
 * Vehicle.cpp
 *
 *  Created on: 13 dic. 2022
 *      Author: aleja
 */

#include "Vehicle/Vehicle.hpp"

StateMachine Vehicle::state_machine = StateMachine(OPERATIONAL);

void Vehicle::initialize(uint16_t transition_check_time_in_milliseconds) {
	Vehicle::state_machine.add_state(SLEEP);
	Vehicle::state_machine.add_state(FAULT);

	optional<uint8_t> optional_id = Time::register_high_precision_alarm(transition_check_time_in_milliseconds, []() {
		Vehicle::state_machine.check_transitions();
	});
	if (not optional_id) {
		//TODO: Error Handler
	}
}

uint8_t Vehicle::add_cyclic_action(uint16_t time, function<void()> action, States state, TimeUnit time_unit) {
	function<void()> state_action = [action, state](){
		if (state_machine.current_state == state){
			action();
		}
	};
	optional<uint8_t> optional_id;

	if (time_unit == SECONDS) {
		optional_id = Time::register_low_precision_alarm(time*1000, state_action);
	}

	else if (time_unit == MILLISECONDS) {
		optional_id = Time::register_low_precision_alarm(time, state_action);
	}

	else if (time_unit == MICROSECONDS) {
		optional_id = Time::register_high_precision_alarm(time, state_action);
	}

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
