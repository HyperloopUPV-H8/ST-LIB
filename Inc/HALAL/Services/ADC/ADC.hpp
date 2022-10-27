/*
 * ADC.hpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#pragma once

#include "../../Models/PinModel/Pin.hpp"
#include "../../../C++Utilities/CppUtils.hpp"


struct ADCchannel {
	ADC_HandleTypeDef* adc;
	uint8_t rank;
};

class ADC {
public:
	static map<uint8_t, Pin> serviceIDs;

	static map<Pin, ADCchannel> pinTimerMap;
	static forward_list<uint8_t> IDmanager;

	static optional<uint8_t> register_adc(Pin pin);
	static void turn_on_adc(uint8_t id);
	static void turn_off_adc(uint8_t id);
	static optional<uint16_t> get_pin_value(uint8_t id);
};
