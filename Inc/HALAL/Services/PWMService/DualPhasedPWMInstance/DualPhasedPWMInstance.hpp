/*
 * DualPhasedPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWMService/PhasedPWMInstance/PhasedPWMInstance.hpp"
#include "PWMService/DualPWMInstance/DualPWMInstance.hpp"

class DualPhasedPWMInstance : virtual public PhasedPWMInstance, DualPWMInstance {};
