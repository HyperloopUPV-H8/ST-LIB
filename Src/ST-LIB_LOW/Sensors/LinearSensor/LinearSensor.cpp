#include "Sensors/LinearSensor/LinearSensor.hpp"
#include "Sensors/Sensor/Sensor.hpp"

LinearSensor::LinearSensor(Pin pin, double slope, double offset, double *value)
: slope(slope), offset(offset), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(not identification){
		ErrorHandler((string)"  The pin is already used or isn t available for ADC usage" ,pin);
		return;
	}
	id = identification.value();
	Sensor::adc_id_list.insert(Sensor::adc_id_list.begin(),id);

}

void LinearSensor::read(){
	optional<float> val = ADC::get_value(id);
	if(not val){
		ErrorHandler((string)" Couldn t read the value of the ADC pin" ,pin);
		return;
	}
	*value = slope * (double) val.value() + offset;
}

uint8_t LinearSensor::get_id(){
	return id;
}
