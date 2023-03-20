#include "Sensors/LinearSensor/LinearSensor.hpp"
#include "Sensors/Sensor/Sensor.hpp"

LinearSensor::LinearSensor(Pin &pin, float *value, float slope, float offset)
: pin(pin), slope(slope), offset(offset), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(not identification){
		ErrorHandler("The pin %s is already used or isn t available for ADC usage", pin.to_string().c_str());
		return;
	}
	id = identification.value();
	Sensor::adc_id_list.insert(Sensor::adc_id_list.begin(),id);

}

LinearSensor::LinearSensor(Pin &pin, float &value, float slope, float offset) : LinearSensor::LinearSensor(pin, &value, slope,offset){}

void LinearSensor::read(){
	optional<float> val = ADC::get_value(id);
	if(not val){
		ErrorHandler("Could not read ADC value of pin %s", pin.to_string().c_str());
		return;
	}
	*value = slope * (double) val.value() + offset;
}

uint8_t LinearSensor::get_id(){
	return id;
}
