/*
 * Created by Alejandro
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>
#include "Time/Time.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

typedef std::chrono::milliseconds ms;
typedef std::chrono::microseconds us;
typedef std::chrono::seconds s;

enum AlarmType{
	LOW_PRECISION,
	MID_PRECISION,
	HIGH_PRECISION
};

class TimedAction {
public:
	function<void()> action;
	uint32_t period;
	AlarmType alarm_precision;
	uint8_t id = -1;

	TimedAction() = default;
};

class State {
public:
	vector<TimedAction> cyclic_actions;
	vector<function<void()>> on_enter_actions = {};
	vector<function<void()>> on_exit_actions = {};
	void enter();
	void exit();
	template<class TimeUnit>
	void add_new_timed_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period);
	template<class TimeUnit>
	void register_new_timed_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period);
	void unregister_all_timed_actions();
	void register_all_timed_actions();
};

template<class TimeUnit>
void State::register_new_timed_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, AlarmType precision_type){
	TimedAction timed_action = {};
	timed_action.alarm_precision = precision_type;
	timed_action.action = action;
	switch (precision_type) {
		case LOW_PRECISION:
			uint32_t miliseconds = chrono::duration_cast<chrono::milliseconds>(period).count();
			timed_action.period = miliseconds;
			timed_action.id = Time::register_low_precision_alarm(miliseconds, action);
			break;
		case MID_PRECISION:
			uint32_t microseconds = chrono::duration_cast<chrono::microseconds>(period).count();
			timed_action.period = microseconds;
			timed_action.id = Time::register_mid_precision_alarm(microseconds, action).value();
			break;
		case HIGH_PRECISION:
			uint32_t microseconds = chrono::duration_cast<chrono::microseconds>(period).count();
			timed_action.period = microseconds;
			timed_action.id = Time::register_high_precision_alarm(microseconds, action).value();
			break;
		default:
			ErrorHandler("Alarm Precision Type does not exist, AlarmType: %d", precision_type);
			return;
			break;
	}
	cyclic_actions.push_back(timed_action);
}

template<class TimeUnit>
void State::add_new_timed_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, AlarmType precision_type){
	TimedAction timed_action = {};
	timed_action.alarm_precision = precision_type;
	timed_action.action = action;
	switch (precision_type) {
		case LOW_PRECISION:
			uint32_t miliseconds = chrono::duration_cast<chrono::milliseconds>(period).count();
			timed_action.period = miliseconds;
			break;
		case MID_PRECISION:
			uint32_t microseconds = chrono::duration_cast<chrono::microseconds>(period).count();
			timed_action.period = microseconds;
			break;
		case HIGH_PRECISION:
			uint32_t microseconds = chrono::duration_cast<chrono::microseconds>(period).count();
			timed_action.period = microseconds;
			break;
		default:
			ErrorHandler("Alarm Precision Type does not exist, AlarmType: %d", precision_type);
			return;
			break;
	}
	cyclic_actions.push_back(timed_action);
}

void State::unregister_all_timed_actions(){
	for(const TimedAction& timed_action : cyclic_actions){
		switch(timed_action.alarm_precision){
		case LOW_PRECISION:
			Time::unregister_low_precision_alarm(timed_action.id);
			break;
		case MID_PRECISION:
			Time::unregister_mid_precision_alarm(timed_action.id);
			break;
		case HIGH_PRECISION:
			Time::unregister_high_precision_alarm(timed_action.id);
			break;
		default:
			ErrorHandler("Cannot unregister timed action with erroneus alarm precision, Alarm Precision Type: %d", timed_action.alarm_precision);
			break;
		}
	}
}

void State::register_all_timed_actions(){
	for(TimedAction& timed_action : cyclic_actions){
		switch(timed_action.alarm_precision){
		case LOW_PRECISION:
			timed_action.id = Time::register_low_precision_alarm(timed_action.period, timed_action.action);
			break;
		case MID_PRECISION:
			timed_action.id = Time::register_mid_precision_alarm(timed_action.period, timed_action.action);
			break;
		case HIGH_PRECISION:
			timed_action.id = Time::register_high_precision_alarm(timed_action.period, timed_action.action);
			break;
		default:
			ErrorHandler("Cannot register timed action with erroneus alarm precision, Alarm Precision Type: %d", timed_action.alarm_precision);
			break;
		}
	}
}


class StateMachine {
public:
	typedef uint8_t state_id;

	state_id initial_state = 0;
	state_id current_state = 0;


	StateMachine() = default;
	StateMachine(state_id initial_state);

	void add_state(state_id state);
	void add_transition(state_id old_state, state_id new_state, function<bool()> transition);

	template<class TimeUnit>
	void add_low_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period);
	template<class TimeUnit>
	void add_low_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, state_id state);

	template<class TimeUnit>
	void add_mid_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period);
	template<class TimeUnit>
	void add_mid_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, state_id state);

	template<class TimeUnit>
	void add_high_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period);
	template<class TimeUnit>
	void add_high_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, state_id state);

	void add_enter_action(function<void()> action);
	void add_enter_action(function<void()> action, state_id state);

	void add_exit_action(function<void()> action);
	void add_exit_action(function<void()> action, state_id state);

	void force_change_state(state_id new_state);
	void check_transitions();

	void add_state_machine(StateMachine& state_machine, state_id state);


private:
	unordered_map<state_id, State> states;
	unordered_map<state_id, unordered_map<state_id, function<bool()>>> transitions;
	unordered_map<state_id, StateMachine*> nested_state_machine;
};

template<class TimeUnit>
void StateMachine::add_low_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, state_id state) {
	if (not states.contains(state)) {
		ErrorHandler("The state %d is not added to the state machine", state);
		return;
	}

	uint32_t microseconds = (uint32_t)chrono::duration_cast<chrono::microseconds>(period).count();

	if(microseconds % 1000 != 0){
		ErrorHandler("Low precision cyclic action does not have enough resolution for the desired period, Desired period: %d uS", microseconds);
		return;
	}

	if(state == current_state) states[state].register_new_timed_action(action, period, LOW_PRECISION);
	else states[state].add_new_timed_action(action, period,LOW_PRECISION);
}

template<class TimeUnit>
void StateMachine::add_low_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period){
	add_low_precision_cyclic_action(action, period, current_state);
}

template<class TimeUnit>
void StateMachine::add_mid_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, state_id state) {
	if (not states.contains(state)) {
		ErrorHandler("The state %d is not added to the state machine", state);
		return;
	}

	uint32_t microseconds = (uint32_t)chrono::duration_cast<chrono::microseconds>(period).count();

	if(microseconds % 50 != 0){
		ErrorHandler("Mid precision cyclic action does not have enough resolution for the desired period, Desired period: %d uS", microseconds);
		return;
	}

	if(state == current_state) states[state].register_new_timed_action(action, period, MID_PRECISION);
	else states[state].add_new_timed_action(action, period,MID_PRECISION);
}

template<class TimeUnit>
void StateMachine::add_mid_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period){
	add_mid_precision_cyclic_action(action, period, current_state);
}

template<class TimeUnit>
void StateMachine::add_high_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, state_id state) {
	if (not states.contains(state)) {
		ErrorHandler("The state %d is not added to the state machine", state);
		return;
	}

	uint32_t microseconds = (uint32_t)chrono::duration_cast<chrono::microseconds>(period).count();

	if(microseconds < 1){
		ErrorHandler("High precision cyclic action does not have enough resolution for the desired period, Desired period: %d uS", microseconds);
		return;
	}

	if(state == current_state) states[state].register_new_timed_action(action, period, HIGH_PRECISION);
	else states[state].add_new_timed_action(action, period,HIGH_PRECISION);
}

template<class TimeUnit>
void StateMachine::add_high_precision_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period){
	add_high_precision_cyclic_action(action, period, current_state);
}

#endif
