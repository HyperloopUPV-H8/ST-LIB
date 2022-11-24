#include "ST-LIB_LOW/Sensors/LinearSensor/LinearSensor.hpp"
#include "ADC/ADC.hpp"

LinearSensor::LinearSensor(Pin pin, double slope, double offset, double *value)
: slope(slope), offset(offset), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(identification){
		id = identification.value();
	}else{
		//TODO: add Error handler for register here (register returns empty optional)
		id = 69;
	}
}

void LinearSensor::start(){
	ADC::turn_on(id);
}

void LinearSensor::read(){
	optional<float> val = ADC::get_value(id);
	if(val){
		*value = slope * (double) val.value() + offset;
	}else{
		//TODO: add Error handler for read here (read returns empty optional)
	}
}

uint8_t LinearSensor::get_id(){
	return id;
}
