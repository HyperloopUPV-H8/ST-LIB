#include "ST-LIB_LOW/Sensors/DigitalSensor/DigitalSensor.hpp"
#include "DigitalInput/DigitalInput.hpp"


DigitalSensor::DigitalSensor(Pin pin, PinState *value) : id(DigitalInput::register_digital_input(pin)), value(value){
}

void DigitalSensor::exti_interruption(){

}

void DigitalSensor::read(){
	*value = DigitalInput::read_pin_state(id).value();

}

uint8_t DigitalSensor::getID(){
	return id;
}
