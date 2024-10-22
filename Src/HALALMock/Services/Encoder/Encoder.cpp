/*
 * Encoder.cpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#include "HALALMock/Services/Encoder/Encoder.hpp"
#include "HALALMock/Models/TimerPeripheral/TimerPeripheral.hpp"
#ifdef HAL_TIM_MODULE_ENABLED

map<uint8_t, pair<Pin, Pin>> Encoder::registered_encoder = {};

uint8_t Encoder::id_counter = 0;

uint8_t Encoder::inscribe(Pin& pin1, Pin& pin2) {
    pair<Pin, Pin> doublepin = {pin1, pin2};
    if (not Encoder::pin_timer_map.contains(doublepin)) {
        ErrorHandler(
            " The pin %s and the pin %s are already used or aren't "
            "configurated for encoder usage",
            pin1.to_string().c_str(), pin2.to_string().c_str());
        return 0;
    }
    uint8_t id = Encoder::id_counter++;
    Encoder::registered_encoder[id] = doublepin;

    EmulatedPin &pin1_data = SharedMemory::get_pin(pin1);
    EmulatedPin &pin2_data = SharedMemory::get_pin(pin2);

	if(pin_data1.type == PinType::NOT_USED && pin2_data.type == PinType::NOT_USED){
		pin1_data.type = PinType::ENCODER;
        pin2_data.type = PinType::ENCODER;
        direction = &(pin1_data.PinData.ENCODER.direction);
		count_value = &(pin1_data.PinData.ENCODER.count_value);
        is_on = &(pin1_data.PinData.ENCODER.is_on);
        //initialize by default values
        *direction = false;
        *count_value = 0;
        is_on = false;
    return id;
}

void Encoder::start(){}

void Encoder::turn_on(uint8_t id) {
    if (not Encoder::registered_encoder.contains(id)) {
        ErrorHandler("No encoder registered with id %u", id);
        return;
    }
    *is_on = true;
}

void Encoder::turn_off(uint8_t id) {
    if (not Encoder::registered_encoder.contains(id)) {
        ErrorHandler("No encoder registered with id %u", id);
        return;
    }
    *is_on = false;
}

void Encoder::reset(uint8_t id) {}

uint32_t Encoder::get_counter(uint8_t id) {
    return *count_value;
}

bool Encoder::get_direction(uint8_t id) {
    if (not Encoder::registered_encoder.contains(id)) {
        ErrorHandler("No encoder registered with id %u", id);
        return false;
    }
    return *direction;
}

uint32_t Encoder::get_initial_counter_value(uint8_t id) {
    return 0;
}

void Encoder::init(TimerPeripheral* encoder) {}
}

#endif
