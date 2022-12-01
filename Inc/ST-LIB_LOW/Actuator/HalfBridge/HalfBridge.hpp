/*
 * HalfBridge.hpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#include "Actuator/PWM/PWM.hpp"
#include "Actuator/DigitalOutput/DigitalOutput.hpp"

class HalfBridge {
public:
	HalfBridge(Pin pin_A, Pin pin_B);
	HalfBridge(Pin pin_A, Pin pin_A_negated, Pin pin_B, Pin pin_B_negated);

	void set_duty_cycle(uint8_t duty_cycle);

private:
	enum negated_implementation {
		HARDWARE,
		FIRMWARE
	};

	Pin pin_A;
	Pin pin_A_negated;
	Pin pin_B;
	Pin pin_B_negated;
};
