///*
// * HalfBridge.cpp
// *
// *  Created on: Dec 1, 2022
// *      Author: aleja
// */
//
//#include "Actuator/HalfBridge/HalfBridge.hpp"
//
//HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& negative_pwm_pin, Pin& enable_pin){
//	PhasedPWMInstance positive_pwm(positive_pwm_pin);
//	PhasedPWMInstance negative_pwm(negative_pwm_pin);
//
//	optional<uint8_t> enable_id = DigitalOutputService::inscribe(enable_pin);
//	if (not enable_id) {
//		ErrorHandler("Unable to inscribe the HalfBridge enable pin in %s", enable_pin.to_string());
//	}
//	HalfBridge::enable = enable_id.value();
//}
//
//HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin,
//		Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin, Pin& enable_pin) {
//	DualPhasedPWMInstance positive_pwm(positive_pwm_pin);
//	DualPhasedPWMInstance negative_pwm(negative_pwm_pin);
//
//	optional<uint8_t> enable_id = DigitalOutputService::inscribe(enable_pin);
//	if (not enable_id) {
//		ErrorHandler("Unable to inscribe the HalfBridge enable pin in %s", enable_pin.to_string());
//	}
//	HalfBridge::enable = enable_id.value();
//}
//
//void HalfBridge::turn_on() {
//	positive_pwm.turn_on();
//	negative_pwm.turn_on();
//	DigitalOutputService::turn_on(enable); // enable al final para evitar ruido
//}
//
//void HalfBridge::turn_off() {
//	DigitalOutputService::turn_off(enable); // enable al principio para evitar ruido
//	positive_pwm.turn_off();
//	negative_pwm.turn_off();
//}
//
//void HalfBridge::set_duty_cycle(int8_t duty_cycle) {
//	if (duty_cycle >= 0) {
//		positive_pwm.set_duty_cycle(duty_cycle);
//		negative_pwm.set_duty_cycle(0);
//	}
//	else {
//		positive_pwm.set_duty_cycle(0);
//		negative_pwm.set_duty_cycle(abs(duty_cycle));
//	}
//}
