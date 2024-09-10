
/*
 * SensorInterrupt.hpp
 *
 *  Created on: June 21, 2023
 *      Author: Alejandro
 */

#pragma once
#include "EXTI/EXTI.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "HALAL/Services/InputCapture/InputCapture.hpp"
template<class Type>
class PWMSensor {
protected:
	uint8_t id;
	Type *duty_cycle;
	Type *frequency;

public:
	PWMSensor() = default;
	PWMSensor(Pin &pin, Type &frequency, Type &duty_cycle);
	PWMSensor(Pin &pin, Type *frequency, Type *duty_cycle);
	void read();
	uint8_t get_id();
};

template<class Type>
PWMSensor<Type>::PWMSensor(Pin &pin, Type &frequency, Type &duty_cycle) :
	frequency(&frequency), duty_cycle(&duty_cycle) {
		id = InputCapture::inscribe(pin);
}

template<class Type>
PWMSensor<Type>::PWMSensor(Pin &pin, Type *frequency, Type *duty_cycle) :
	duty_cycle(duty_cycle), frequency(frequency) {
		id = InputCapture::inscribe(pin);
}

template<class Type>
void PWMSensor<Type>::read() {
	*duty_cycle = InputCapture::read_duty_cycle(id);
	*frequency = InputCapture::read_frequency(id);
}

