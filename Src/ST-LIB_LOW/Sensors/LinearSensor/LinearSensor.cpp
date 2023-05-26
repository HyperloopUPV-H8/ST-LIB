#include "Sensors/LinearSensor/LinearSensor.hpp"
#include "Sensors/Sensor/Sensor.hpp"

LinearSensor::LinearSensor(Pin &pin, double slope, double offset, double *value)
: pin(pin), slope(slope), offset(offset), value(value){
	id = ADC::inscribe(pin);

	Sensor::adc_id_list.push_back(id);
}

LinearSensor::LinearSensor(Pin &pin, double slope, double offset, double &value):LinearSensor::LinearSensor(pin,slope,offset,&value){}

void LinearSensor::read(){
	float val = ADC::get_value(id);

	*value = slope * (double) val + offset;
}

uint8_t LinearSensor::get_id(){
	return id;
}
