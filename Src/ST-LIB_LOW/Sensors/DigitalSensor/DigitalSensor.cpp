#include "Sensors/DigitalSensor/DigitalSensor.hpp"
#include "Sensors/Sensor/Sensor.hpp"


DigitalSensor::DigitalSensor(Pin &pin, PinState *value) : id(DigitalInput::inscribe(pin)), value(value){}

DigitalSensor::DigitalSensor(Pin &pin, PinState &value) : DigitalSensor::DigitalSensor(pin,&value){}

void DigitalSensor::read(){
	PinState val = DigitalInput::read_pin_state(id);

	*DigitalSensor::value = val;
}

uint8_t DigitalSensor::get_id(){
	return id;
}

uint8_t DigitalSensor::get_value(){
	read();
	if(value != nullptr)
	{
		if(*value == PinState::ON)
			return 1;
		else
			return 0;
	}
}
