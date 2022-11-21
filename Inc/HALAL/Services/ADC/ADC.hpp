/*
 * ADC.hpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#pragma once
#include "ST-LIB-CORE.hpp"

#define ADC_BUF1_LEN 4
#define ADC_BUF2_LEN 2
#define ADC_BUF3_LEN 2

struct ADCchannel {
	ADC_HandleTypeDef* adc;
	uint8_t rank;
};

class ADC {
public:
	static map<uint8_t, Pin> service_ids;

	static map<Pin, ADCchannel> pin_adc_map;
	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> inscribe(Pin pin);
	static void turn_on(uint8_t id);
	static optional<uint16_t> get_value(uint8_t id);
};
