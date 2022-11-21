/*
 * ADC.hpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#pragma once
#include "ST-LIB.hpp"
#include "C++Utilities/CppUtils.hpp"

#define ADC_BUF1_LEN 4
#define ADC_BUF2_LEN 2
#define ADC_BUF3_LEN 2

#define LPTIM1_PERIOD 3000
#define LPTIM2_PERIOD 3000
#define LPTIM3_PERIOD 3000

#define MAX_VOLTAGE 3.3
#define MAX_12BIT 4095.0
#define MAX_16BIT 65535.0

struct buffer { // TODO Will be moved into appropiate file when include structure is fixed.
	uint16_t* data;
	uint8_t length;
};

struct low_power_timer {
	LPTIM_HandleTypeDef* timer;
	uint16_t period;
};
class ADC {
public:
	class Instance {
	public:
		ADC_HandleTypeDef* adc;
		uint8_t rank;

		Instance() = default;
		Instance(ADC_HandleTypeDef* adc, uint8_t rank);
	};

	static map<ADC_HandleTypeDef*, low_power_timer> low_power_timers;
	static map<ADC_HandleTypeDef*, buffer> buffers;

	static map<Pin, Instance> available_instances;
	static map<uint8_t, Instance> active_instances;
	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> inscribe(Pin pin);
	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static optional<float> get_value(uint8_t id);
};
