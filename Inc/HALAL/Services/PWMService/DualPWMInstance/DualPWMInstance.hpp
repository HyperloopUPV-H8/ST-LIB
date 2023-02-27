/*
 * DualPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWMService/PWMInstance/PWMInstance.hpp"

class DualPWMInstance : virtual public PWMInstance {
private:
	void turn_on();
	void turn_off();
};
