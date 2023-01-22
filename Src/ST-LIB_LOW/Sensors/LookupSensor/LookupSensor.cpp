#include "ST-LIB_LOW/Sensors/LookupSensor/LookupSensor.hpp"
#include "ADC/ADC.hpp"
#include "C++Utilities/CppUtils.hpp"
#include <iostream>


LookupSensor::LookupSensor(Pin pin, double *table, int table_size, double *value) : table(table), table_size(table_size), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(not identification){
		//TODO: add Error handler for register here (register returns empty optional)
		return;
	}
	id = identification.value();
	Sensor::adc_id_list.insert(Sensor::adc_id_list.begin(),id);
}

void LookupSensor::read(){
	optional<float> adc_voltage = ADC::get_value(id);
	if(not adc_voltage){
		//TODO: add Error handler for read here (read returns empty optional)
		return;
	}

	int table_index = (int)(adc_voltage.value() * table_size / REFERENCE_VOLTAGE);
	if(table_index >= table_size){
		table_index = table_size - 1;
	}
	*value = table[table_index];
}

uint8_t LookupSensor::get_id(){
	return id;
}
