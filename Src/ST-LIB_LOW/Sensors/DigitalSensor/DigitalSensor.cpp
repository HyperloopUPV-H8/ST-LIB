#include "ST-LIB_LOW/Sensors/DigitalSensor/DigitalSensor.hpp"
#include "DigitalInput/DigitalInput.hpp"
#include "EXTI/EXTI.hpp"

//digitalInput doesn t return an optional in register. Instead, it returns the value directly (fault free?)
DigitalSensor::DigitalSensor(Pin pin, PinState *value) : pin(pin), id(DigitalInput::register_digital_input(pin)), value(value){
}

void DigitalSensor::exti_interruption(std::function<auto> lambda){
	optional<uint8_t> identification = ExtI::register_exti(pin, lambda);
	if(identification){
		ExtI::turn_on_exti(identification.value());
	}else{
		//errores de optional vacio aqui (por hacer)
	}

}

void DigitalSensor::read(){
	optional<PinState> val = DigitalInput::read_pin_state(id);
	if(val){
		*value = val.value();
	}else{
		//errores de optional vacio aqui (por hacer)
	}

}

uint8_t DigitalSensor::getID(){
	return id;
}
