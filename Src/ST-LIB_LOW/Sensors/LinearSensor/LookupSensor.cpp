#include "ST-LIB_LOW/Sensors/LinearSensor/LookupSensor.hpp"
#include "ADC/ADC.hpp"

LookupSensor::LookupSensor(Pin pin, double *table) : id(ADC::register_adc(pin).value()), table(table){
	ADC::turn_on_adc(id);
}

void LookupSensor::read(double &value){

	value = table[ADC::get_pin_value(id).value()];
}
