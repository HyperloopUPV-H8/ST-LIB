/*
 * HalfBridge.cpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#include "Actuator/HalfBridge/HalfBridge.hpp"

HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& negative_pwm_pin, Pin& enable_pin){
	HalfBridge::positive_pwm = PWMservice::inscribe(positive_pwm_pin);
	HalfBridge::positive_pwm = PWMservice::inscribe(negative_pwm_pin);
	HalfBridge::enable = DigitalOutputservice::inscribe(enable_pin);

}

HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin,
		Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin, Pin& enable_pin) {
	HalfBridge::positive_pwm = PWMservice::inscribe_dual(positive_pwm_pin, positive_pwm_negated_pin);
	HalfBridge::negative_pwm = PWMservice::inscribe_dual(negative_pwm_pin, negative_pwm_negated_pin);
	HalfBridge::enable = DigitalOutputservice::inscribe(enable_pin);
}

void HalfBridge::turn_on() {
	PWMservice::turn_on(positive_pwm);
	PWMservice::turn_on(negative_pwm);
	DigitalOutputservice::turn_on(enable); // enable al final para evitar ruido
}

void HalfBridge::turn_off() {
	DigitalOutputservice::turn_off(enable); // enable al principio para evitar ruido
	PWMservice::turn_off(positive_pwm);
	PWMservice::turn_off(negative_pwm);
}

void HalfBridge::set_duty_cycle(int8_t duty_cycle) {
	if (duty_cycle >= 0) {
		PWMservice::set_duty_cycle(negative_pwm, 0);
		PWMservice::set_duty_cycle(positive_pwm, duty_cycle);
	}
	else {
		PWMservice::set_duty_cycle(positive_pwm, 0);
		PWMservice::set_duty_cycle(negative_pwm, duty_cycle);
	}
}
