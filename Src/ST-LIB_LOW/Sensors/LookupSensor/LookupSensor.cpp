#include "ST-LIB_LOW/Sensors/LookupSensor/LookupSensor.hpp"
#include "ADC/ADC.hpp"
#include "C++Utilities/CppUtils.hpp"
#include <iostream>


LookupSensor::LookupSensor(Pin pin, double *table, int table_size, double *value) : table(table), table_size(table_size), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(identification){
		id = identification.value();
		ADC::turn_on(id);
	}else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

void LookupSensor::read(){
	optional<uint16_t> val = ADC::get_value(id);
	if(val){
		*value = table[(int)(2.3 * table_size / reference_voltage)];
	}else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

uint8_t LookupSensor::getID(){
	return id;
}
