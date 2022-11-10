#include "ST-LIB_LOW/Sensors/DigitalSensor/DigitalSensor.hpp"
#include "DigitalInput/DigitalInput.hpp"

//digitalInput no usa optionals para el register
DigitalSensor::DigitalSensor(Pin pin, PinState *value) : id(DigitalInput::register_digital_input(pin)), value(value){
}

void DigitalSensor::exti_interruption(){

}

void DigitalSensor::read(){
	optional<PinState> val = DigitalInput::read_pin_state(id);
	if(val){
		*value = val.value();
	}else{

	}

}

uint8_t DigitalSensor::getID(){
	return id;
}
