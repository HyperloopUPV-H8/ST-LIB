#include "ST-LIB_LOW/Sensors/LinearSensor/LinearSensor.hpp"
#include "ADC/ADC.hpp"

LinearSensor::LinearSensor(Pin pin, double slope, double offset) : id(ADC::register_adc(pin).value()), slope(slope), offset(offset){
	ADC::turn_on_adc(id);
}

/*void LinearSensor::read(double &value){

	value = slope * (float) ADC::get_pin_value(id).value() + offset;
}*/
