#include "ST-LIB_LOW/Sensors/LookupSensor/LookupSensor.hpp"
#include "ADC/ADC.hpp"
#include "C++Utilities/CppUtils.hpp"
#include <iostream>


LookupSensor::LookupSensor(Pin pin, double *table, int table_size, double *value) : table(table), table_size(table_size), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(identification){
		id = identification.value();
	}else{
		//TODO: add Error handler for register here (register returns empty optional)
	}
}

void LookupSensor::start(){
	ADC::turn_on(id);
}

void LookupSensor::read(){
	optional<float> val = ADC::get_value(id);
	if(val){
		int table_index = (int)(val.value() * table_size / REFERENCE_VOLTAGE);
		if(table_index >= table_size){table_index = table_size - 1;}
		*value = table[table_index];
	}else{
		//TODO: add Error handler for read here (read returns empty optional)
	}
}

uint8_t LookupSensor::get_id(){
	return id;
}
