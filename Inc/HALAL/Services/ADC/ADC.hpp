/*
 * ADC.hpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#pragma once
#include "PinModel/Pin.hpp"
#include "LowPowerTimer/LowPowerTimer.hpp"

#ifdef HAL_ADC_MODULE_ENABLED

#define ADC_BUF_LEN 16
#define LPTIM1_PERIOD 3000
#define LPTIM2_PERIOD 3000
#define LPTIM3_PERIOD 3000

#define MAX_VOLTAGE 3.3
#define MAX_12BIT 4095.0
#define MAX_16BIT 65535.0

class ADC {
public:
	struct InitData {
	public:
		ADC_TypeDef* adc;
		uint32_t resolution;
		uint32_t external_trigger;
		vector<uint32_t> channels;
		InitData() = default;
		InitData(ADC_TypeDef* adc, uint32_t resolution, uint32_t external_trigger, vector<uint32_t>& channels);
	};

	class Peripheral {
	public:
		ADC_HandleTypeDef* handle;
		uint16_t* dma_stream;
		LowPowerTimer timer;
		InitData init_data;
		bool is_on = false;

		Peripheral() = default;
		Peripheral(ADC_HandleTypeDef* handle, uint16_t* dma_stream, LowPowerTimer& timer, InitData& init_data);

		bool is_registered();
	};

	class Instance {
	public:
		Peripheral* peripheral;
		uint32_t channel;
		uint32_t rank;

		Instance() = default;
		Instance(Peripheral* peripheral, uint32_t channel);
	};

	static optional<uint8_t> inscribe(Pin pin);
	static void start();
	static void turn_on(uint8_t id);
	static optional<float> get_value(uint8_t id);

private:
	static uint32_t ranks[16];
	static Peripheral peripherals[3];
	static map<Pin, Instance> available_instances;
	static map<uint8_t, Instance> active_instances;
	static forward_list<uint8_t> id_manager;

	static void init(Peripheral& peripheral);
};

#endif
