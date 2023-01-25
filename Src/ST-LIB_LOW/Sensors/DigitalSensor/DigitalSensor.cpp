#include "Sensors/DigitalSensor/DigitalSensor.hpp"
#include "Sensors/Sensor/Sensor.hpp"
#define NAME_OF( v ) #v

DigitalSensor::DigitalSensor(Pin pin, PinState *value) : pin(pin), id(DigitalInput::inscribe(pin)), value(value){}

void DigitalSensor::exti_interruption(std::function<void()> &&action){
	optional<uint8_t> identification = ExternalInterrupt::inscribe(pin, std::move(action));
	if (not identification) {
		ErrorHandler((string)" The pin is already used or isn t available for EXTI usage" ,pin);
		return;
	}
	exti_id = identification.value();
	Sensor::EXTI_id_list.insert(Sensor::EXTI_id_list.begin(),exti_id);
}

void DigitalSensor::read(){
	optional<PinState> val = DigitalInput::read_pin_state(id);
	if(not val){
		ErrorHandler((string)" Couldn t read the state of the pin" ,pin);
		return;
	}
	*value = val.value();
}
