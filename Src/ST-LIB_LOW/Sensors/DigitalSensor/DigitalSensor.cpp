#include "Sensors/DigitalSensor/DigitalSensor.hpp"
#include "Sensors/Sensor/Sensor.hpp"


DigitalSensor::DigitalSensor(Pin &pin, PinState *value) : pin(pin), id(DigitalInput::inscribe(pin)), value(value){}

DigitalSensor::DigitalSensor(Pin &pin, PinState &value) : DigitalSensor::DigitalSensor(pin,&value){}

void DigitalSensor::exti_interruption(std::function<void()> &&action){
	optional<uint8_t> identification = ExternalInterrupt::inscribe(pin, std::move(action));
	if (not identification) {
		ErrorHandler(" The pin %s is already used or isn t available for EXTI usage", pin.to_string());
		return;
	}
	exti_id = identification.value();
	Sensor::EXTI_id_list.insert(Sensor::EXTI_id_list.begin(),exti_id);
}

void DigitalSensor::read(){
	optional<PinState> val = DigitalInput::read_pin_state(id);
	if(not val){
		ErrorHandler("Can not read the state of the pin %s", pin.to_string());
		return;
	}
	*value = val.value();
}

uint8_t DigitalSensor::get_id(){
	return id;
}
