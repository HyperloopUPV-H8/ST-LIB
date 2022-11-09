#include "ST-LIB_LOW/Sensors/LinearSensor/LookupSensor.hpp"
#include "ADC/ADC.hpp"

LookupSensor::LookupSensor(Pin pin, double *table, double *value) : id(ADC::register_adc(pin).value()), table(table), value(value){
	ADC::turn_on_adc(id);
}

void LookupSensor::read(){

	*value = table[ADC::get_pin_value(id).value()];
}
