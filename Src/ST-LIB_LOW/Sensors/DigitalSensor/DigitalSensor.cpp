#include "ST-LIB_LOW/Sensors/DigitalSensor/DigitalSensor.hpp"
#include "DigitalInputService/DigitalInputService.hpp"
#include "EXTI/EXTI.hpp"

DigitalSensor::DigitalSensor(Pin pin, PinState *value) : pin(pin), id(DigitalInput::inscribe(pin)), value(value){}

void DigitalSensor::exti_interruption(std::function<auto> action){
	optional<uint8_t> identification = ExternalInterrupt::inscribe(pin, action);
	if (not identification) {
		//TODO: add Error handler for register here (register returns empty optional)
		return;
	}
	ExternalInterrupt::turn_on(identification.value());
}

void DigitalSensor::start(){
	//TODO: discuss if it needs start for consistency or not (doesnt have a use for it)
}

void DigitalSensor::read(){
	optional<PinState> val = DigitalInput::read_pin_state(id);
	if(not val){
		//TODO: add Error handler for read here (read returns empty optional)
		return;
	}
	*value = val.value();
}

uint8_t DigitalSensor::get_id(){
	return id;
}
