#include "ST-LIB_LOW/Sensors/DigitalSensor/DigitalSensor.hpp"
#include "DigitalInput/DigitalInput.hpp"


DigitalSensor::DigitalSensor(Pin pin) : id(DigitalInput::register_digital_input(pin)){
}

void DigitalSensor::exti_interruption(){

}

void DigitalSensor::read(PinState &value){
	value = DigitalInput::read_pin_state(id).value();

}

uint8_t DigitalSensor::getID(){
	return id;
}
