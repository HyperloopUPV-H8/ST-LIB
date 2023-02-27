/*
 * PhasedPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWMService/PWMInstance/PWMInstance.hpp"

class PhasedPWMInstance : virtual public PWMInstance {
private:
	uint16_t phase;

	void set_duty_cycle(uint8_t duty_cycle);
};
