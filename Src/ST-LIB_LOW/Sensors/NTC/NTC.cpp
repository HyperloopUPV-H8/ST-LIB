#include "Sensors/NTC/NTC.hpp"

NTC::NTC(Pin &pin){
    id = ADC::inscribe(pin);

    Sensor::adc_id_list.push_back(id);
}

uint8_t NTC::get_id(){
    return id;
}

float NTC::read() {
    int val = ADC::get_int_value(id) >> 4;

    return NTC_table[val] * 0.1;

    
}