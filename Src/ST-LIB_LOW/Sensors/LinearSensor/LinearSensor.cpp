#include "ST-LIB_LOW/Sensors/LinearSensor/LinearSensor.hpp"
#include "ADC/ADC.hpp"

LinearSensor::LinearSensor(Pin pin, double slope, double offset, double *value)
: slope(slope), offset(offset), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(identification){
		id = identification.value();
		ADC::turn_on_adc(id);
	}else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

void LinearSensor::read(){
	optional<uint16_t> val = ADC::get_pin_value(id);
	if(val){
		*value = slope * (double) val.value() + offset;
	}else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

uint8_t LinearSensor::getID(){
	return id;
}
