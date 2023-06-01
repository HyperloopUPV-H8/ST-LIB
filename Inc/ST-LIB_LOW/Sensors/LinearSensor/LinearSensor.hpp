/*
 * LinearSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ADC/ADC.hpp"
#include "Sensors/Sensor/Sensor.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

template<class Type>
class LinearSensor{
public:
	LinearSensor() = default;
	LinearSensor(Pin &pin, Type slope, Type offset, Type *value);
	LinearSensor(Pin &pin, Type slope, Type offset, Type &value);
	void read();
	uint8_t get_id();

protected:
	uint8_t id;
	Type slope;
	Type offset;
	Type *value;
};

template<class Type>
LinearSensor<Type>::LinearSensor(Pin &pin, Type slope, Type offset, Type *value)
: slope(slope), offset(offset), value(value){
	id = ADC::inscribe(pin);

	Sensor::adc_id_list.push_back(id);
}

template<class Type>
LinearSensor<Type>::LinearSensor(Pin &pin, Type slope, Type offset, Type &value):LinearSensor::LinearSensor(pin,slope,offset,&value){}

template<std::integral Type>
void LinearSensor<Type>::read(){
	float val = ADC::get_value(id);

	*value = slope * (Type) val + offset;
}

template<class Type>
uint8_t LinearSensor<Type>::get_id(){
	return id;
}

