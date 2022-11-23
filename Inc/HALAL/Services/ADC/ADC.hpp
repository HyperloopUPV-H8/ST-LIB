/*
 * ADC.hpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#pragma once
#include "PinModel/Pin.hpp"
#include "DMAStream/DMAStream.hpp"
#include "LowPowerTimer/LowPowerTimer.hpp"

#ifdef HAL_ADC_MODULE_ENABLED

#define ADC_BUF1_LEN 10
#define ADC_BUF2_LEN 2
#define ADC_BUF3_LEN 8

#define LPTIM1_PERIOD 3000
#define LPTIM2_PERIOD 3000
#define LPTIM3_PERIOD 3000

#define MAX_VOLTAGE 3.3
#define MAX_12BIT 4095.0
#define MAX_16BIT 65535.0

// TODO: Will be moved into appropiate file when include structure is fixed.




class ADC {
public:
	class Instance {
	public:
		ADC_HandleTypeDef* adc;
		uint8_t rank;
		LowPowerTimer timer;
		DMAStream dma_stream;

		Instance() = default;
		Instance(ADC_HandleTypeDef* adc, uint8_t rank, LowPowerTimer& timer, DMAStream& buffer);
	};

	static map<Pin, Instance> available_instances;
	static map<uint8_t, Instance> active_instances;
	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> inscribe(Pin pin);
	static void start();
	static void turn_on(uint8_t id);
	static optional<float> get_value(uint8_t id);
};

#endif
