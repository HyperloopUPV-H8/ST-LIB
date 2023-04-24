/*
 * ADC.hpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#pragma once
#include <string>

#include "PinModel/Pin.hpp"
#include "LowPowerTimer/LowPowerTimer.hpp"
#include "DMA/DMA.hpp"

#if defined(HAL_ADC_MODULE_ENABLED) && defined(HAL_LPTIM_MODULE_ENABLED)

using std::string;

#define ADC_BUF_LEN 16
#define LPTIM1_PERIOD 3000
#define LPTIM2_PERIOD 3000
#define LPTIM3_PERIOD 3000

#define ADC_MAX_VOLTAGE 3.3
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
		DMA::Stream dma_stream;
		string name;

		InitData() = default;
		InitData(ADC_TypeDef* adc, uint32_t resolution, uint32_t external_trigger, vector<uint32_t>& channels, DMA::Stream dma_stream, string name);
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
	static float get_value(uint8_t id);
	static optional<uint16_t> get_int_value(uint8_t id);

	static Peripheral peripherals[3];

private:
	static uint32_t ranks[16];
	static map<Pin, Instance> available_instances;
	static unordered_map<uint8_t, Instance> active_instances;
	static uint8_t id_counter;

	static void init(Peripheral& peripheral);
};

#endif
