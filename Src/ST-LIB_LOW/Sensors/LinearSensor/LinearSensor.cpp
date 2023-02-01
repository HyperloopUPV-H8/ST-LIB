#include "Sensors/LinearSensor/LinearSensor.hpp"
#include "Sensors/Sensor/Sensor.hpp"

LinearSensor::LinearSensor(Pin pin, double slope, double offset, double *value)
: slope(slope), offset(offset), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(not identification){
		ErrorHandler("The pin %s is already used or isn t available for ADC usage", pin.to_string());
		return;
	}
	id = identification.value();
	Sensor::adc_id_list.insert(Sensor::adc_id_list.begin(),id);

}

void LinearSensor::read(){
	optional<float> val = ADC::get_value(id);
	if(not val){
		ErrorHandler("Could not read ADC value of pin %s", pin.to_string());
		return;
	}
	*value = slope * (double) val.value() + offset;
}

uint8_t LinearSensor::get_id(){
	return id;
}
