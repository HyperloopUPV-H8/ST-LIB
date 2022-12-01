/*
 * HalfBridge.cpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#include "Actuator/HalfBridge/HalfBridge.hpp"

HalfBridge::HalfBridge(Pin pin_A, Pin pin_B) :
	pin_A(pin_A), pin_B(pin_B) {}

HalfBridge::HalfBridge(Pin pin_A, Pin pin_A_negated, Pin pin_B, Pin pin_B_negated) :
		pin_A(pin_A), pin_A_negated(pin_A_negated), pin_B(pin_B), pin_B_negated(pin_B_negated) {}

void set_duty_cycle(uint8_t duty_cycle)
