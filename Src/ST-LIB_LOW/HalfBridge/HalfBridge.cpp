/*
 * HalfBridge.cpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#include <HalfBridge/HalfBridge.hpp>

HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& negative_pwm_pin, Pin& enable_pin) : is_dual(false){
	positive_pwm.p = PhasedPWM(positive_pwm_pin);
	negative_pwm.p = PhasedPWM(negative_pwm_pin);

	optional<uint8_t> enable_id = DigitalOutputService::inscribe(enable_pin);
	if (not enable_id) {
		ErrorHandler("Unable to inscribe the HalfBridge enable pin in %s", enable_pin.to_string());
	}
	HalfBridge::enable = enable_id.value();
}

HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin,
		Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin, Pin& enable_pin) : is_dual(true) {
	positive_pwm.dp = DualPhasedPWM(positive_pwm_pin, positive_pwm_negated_pin);
	negative_pwm.dp = DualPhasedPWM(negative_pwm_pin, negative_pwm_negated_pin);

	optional<uint8_t> enable_id = DigitalOutputService::inscribe(enable_pin);
	if (not enable_id) {
		ErrorHandler("Unable to inscribe the HalfBridge enable pin in %s", enable_pin.to_string());
	}
	HalfBridge::enable = enable_id.value();
}

void HalfBridge::turn_on() {
	if (is_dual) {
		positive_pwm.dp.turn_on();
		negative_pwm.dp.turn_on();
	} else {
		positive_pwm.p.turn_on();
		negative_pwm.p.turn_on();
	}
	DigitalOutputService::turn_on(enable); // enable al final para evitar ruido
}

void HalfBridge::turn_off() {
	DigitalOutputService::turn_off(enable); // enable al principio para evitar ruido
	if (is_dual) {
		positive_pwm.dp.turn_off();
		negative_pwm.dp.turn_off();
	} else {
		positive_pwm.p.turn_off();
		negative_pwm.p.turn_off();
	}
}

void HalfBridge::set_duty_cycle(int8_t duty_cycle) {
	if (is_dual) {
		positive_pwm.dp.set_duty_cycle(duty_cycle);
		negative_pwm.dp.set_duty_cycle(duty_cycle);
	} else {
		positive_pwm.p.set_duty_cycle(duty_cycle);
		negative_pwm.p.set_duty_cycle(duty_cycle);
	}
}
