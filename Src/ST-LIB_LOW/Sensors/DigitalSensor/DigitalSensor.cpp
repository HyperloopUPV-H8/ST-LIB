#include "ST-LIB_LOW/Sensors/DigitalSensor/DigitalSensor.hpp"
#include "DigitalInput/DigitalInput.hpp"
#include "EXTI/EXTI.hpp"

DigitalSensor::DigitalSensor(Pin pin, PinState *value) : pin(pin), id(DigitalInput::register_digital_input(pin)), value(value){
}

void DigitalSensor::exti_interruption(std::function<auto> action){
	optional<uint8_t> identification = ExternalInterrupt::register_exti(pin, action);
	if(identification){
		ExternalInterrupt::turn_on_exti(identification.value());
	}else{
		//TODO: add Error handler for register here (register returns empty optional)
	}

}

void DigitalSensor::start(){
	//TODO: discuss if it needs start for consistency or not (doesnt have a use for it)
}

void DigitalSensor::read(){
	optional<PinState> val = DigitalInput::read_pin_state(id);
	if(val){
		*value = val.value();
	}else{
		//TODO: add Error handler for read here (read returns empty optional)
	}

}

uint8_t DigitalSensor::get_id(){
	return id;
}
