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
		InitData(ADC_TypeDef* adc, uint32_t resolution, uint32_t external_trigger);
	};

	class Peripheral {
	public:
		ADC_HandleTypeDef* handle;
		uint16_t* dma_stream;
		LowPowerTimer timer;
		InitData init_data;
		map<Pin, Channel> available_pins;
		bool is_on = false;

		Peripheral() = default;
		Peripheral(ADC_HandleTypeDef* handle, uint16_t* dma_stream, LowPowerTimer& timer, InitData& init_data, map<Pin, uint32_t>& available_pins);

		bool is_registered() {
			if (init_data.channel_rank_vector.size() == 0) {
				return false;
			}
			else {
				return true;
			}
		}
	};

	class Instance {
	public:
		Peripheral peripheral;
		uint8_t rank;

		Instance() = default;
		Instance(Peripheral& peripheral, uint8_t rank);
	};

	static map<Pin, Instance> available_instances;
	static map<uint8_t, Instance> active_instances;
	static Peripheral peripherals[3];
	static uint32_t ranks[16];
	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> inscribe(Pin pin);
	static void start();
	static void turn_on(uint8_t id);
	static optional<float> get_value(uint8_t id);

	static optional<Pin> get_high_resolution_available_pin();
	static optional<Pin> get_low_resolution_available_pin();

	// private functions (erase this comment)
	static void init(Peripheral& peripheral);
};

#endif
