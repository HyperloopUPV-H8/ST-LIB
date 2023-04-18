#include "Sensors/LookupSensor/LookupSensor.hpp"
#include "Sensors/Sensor/Sensor.hpp"


LookupSensor::LookupSensor(Pin &pin, double *table, int table_size, double *value) : pin(pin), table(table), table_size(table_size), value(value){
	optional<uint8_t> identification = ADC::inscribe(pin);
	if(not identification){
		ErrorHandler("Pin %s is already used or isn t available for ADC usage", pin.to_string().c_str());
		return;
	}
	id = identification.value();
	Sensor::adc_id_list.insert(Sensor::adc_id_list.begin(),id);
}

LookupSensor::LookupSensor(Pin &pin, double *table, int table_size, double &value) : LookupSensor::LookupSensor(pin,table,table_size,&value){}

void LookupSensor::read(){
	optional<float> adc_voltage = ADC::get_value(id);
	if(not adc_voltage){
		ErrorHandler("Can not read ADC value of pin", pin.to_string().c_str());
		return;
	}

	int table_index = (int)(adc_voltage.value() * table_size / REFERENCE_VOLTAGE);
	if(table_index >= table_size){
		table_index = table_size - 1;
	}
	*value = table[table_index];
}

uint8_t LookupSensor::get_id(){
	return id;
}
