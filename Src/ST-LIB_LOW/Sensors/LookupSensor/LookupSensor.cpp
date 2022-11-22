#include "ST-LIB_LOW/Sensors/LookupSensor/LookupSensor.hpp"
#include "ADC/ADC.hpp"

LookupSensor::LookupSensor(Pin pin, double *table, double *value) : table(table), value(value){
	optional<uint8_t> identification = ADC::register_adc(pin);
	if(identification){
		id = identification.value();
		ADC::turn_on_adc(id);
		displacement = sensor_resolution - log2(sizeof(table));
	}else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

void LookupSensor::read(){
	optional<uint16_t> val = ADC::get_pin_value(id);
	if(val){
		*value = table[((uint16_t)val.value())>>displacement];
	}else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

uint8_t LookupSensor::getID(){
	return id;
}
