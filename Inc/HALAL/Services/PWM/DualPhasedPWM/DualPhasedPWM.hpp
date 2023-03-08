/*
 * DualPhasedPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PhasedPWM/PhasedPWM.hpp"
#include "PWM/DualPWM/DualPWM.hpp"

class DualPhasedPWMInstance : virtual public PhasedPWMInstance, DualPWMInstance {};
