#include "ST-LIB_LOW/Sensors/LinearSensor/LinearSensor.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"
#include "ADC/ADC.hpp"

LinearSensor::LinearSensor(Pin pin, double slope, double offset, double *value)
: slope(slope), offset(offset), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(not identification){
		//TODO: add Error handler for register here (register returns empty optional)
		return;
	}
	id = identification.value();
	SensorStarter::adc_id_list.insert(SensorStarter::adc_id_list.begin(),id);

}

void LinearSensor::read(){
	optional<float> val = ADC::get_value(id);
	if(not val){
		//TODO: add Error handler for read here (read returns empty optional)
		return;
	}
	*value = slope * (double) val.value() + offset;
}

uint8_t LinearSensor::get_id(){
	return id;
}
