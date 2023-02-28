/*
 * PWM_Module.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: alejandro 
 */

#pragma once

#include "PinModel/Pin.hpp"
#include "PWMInstance/PWMInstance.hpp"
#include "DualPWMInstance/DualPWMInstance.hpp"
#include "PhasedPWMInstance/PhasedPWMInstance.hpp"
#include "DualPhasedPWMInstance/DualPhasedPWMInstance.hpp"
#include "TimerPeripheral/TimerPeripheral.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

#define H723_TIMERS 24

typedef variant<PWMInstance, DualPWMInstance, PhasedPWMInstance, DualPhasedPWMInstance> PWMvariant;

template <class PWMType>
class PWMservice {
public:
	enum PWM_MODE : uint8_t {
		NORMAL = 0,
		DUAL = 1,
		PHASED = 2,
		PHASED_DUAL = 3
	};

	class Instance {
	public:
		TimerPeripheral* peripheral;
		uint32_t channel;
		PWM_MODE mode;
		float duty_cycle;

		Instance() = default;
		Instance(TimerPeripheral* peripheral, uint32_t channel, PWM_MODE mode);

	};

	static map<uint8_t, pair<PWM_MODE, PWMvariant>> active_instances;
	static map<Pin, PWMInstance> available_instances;
	static map<Pin, PhasedPWMInstance> available_instances_phased;
	static map<pair<Pin, Pin>, DualPWMInstance> available_instances_dual;
	static map<pair<Pin, Pin>, DualPhasedPWMInstance> available_instances_phased_dual;
	static TimerPeripheral timer_peripherals[H723_TIMERS];

    static uint8_t id_counter;

	static optional<uint8_t> inscribe(Pin& pin);
	static optional<uint8_t> inscribe_dual(Pin& pin, Pin& pin_negated);
	static optional<uint8_t> inscribe_phased(Pin& pin);
	static optional<uint8_t> inscribe_dual_phased(Pin& pin);

	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static void set_duty_cycle(uint8_t id, float duty_cycle);
	static void set_frequency(uint8_t id, uint32_t frequency);
private:
	static bool instance_exists(uint8_t id);
	static PWMType& get_instance(uint8_t id);

};

template <class PWMType>
void PWMservice::turn_on(constexpr uint8_t id) {
	if (not instance_exists(id)) {
		ErrorHandler("Instance with id %d does not exist", id);
		return;
	}

	get_instance(id).turn_on();
}

template <class PWMType>
void PWMservice::turn_off(uint8_t id){
	if (not instance_exists(id)) {
		ErrorHandler("Instance with id %d does not exist", id);
		return;
	}

	get_instance(id).turn_off();
}

void PWMservice::set_duty_cycle(uint8_t id, float duty_cycle) {

	if (not (duty_cycle >= 0 && duty_cycle <= 100)) {
		ErrorHandler("The duty cycle of value %d must be in the range [0, 100]", duty_cycle);
		return;
	}
	if (not instance_exists(id)) {
		ErrorHandler("Instance with id %d does not exist", id);
		return;
	}

	get_instance(id).set_duty_cycle();
}
#endif
