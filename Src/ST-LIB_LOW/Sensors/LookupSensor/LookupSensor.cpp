#include "ST-LIB_LOW/Sensors/LookupSensor/LookupSensor.hpp"
#include "ADC/ADC.hpp"

LookupSensor::LookupSensor(Pin pin, double *table, double *value) : table(table), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(identification){
		id = identification.value();
		ADC::turn_on(id);
		displacement = sizeof(table);
	}else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

void LookupSensor::read(){
	optional<uint16_t> val = ADC::get_value(id);
	if(val){
		*value = table[((int)val.value()) * table_size / reference_voltage];
	}else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

uint8_t LookupSensor::getID(){
	return id;
}
