#include "ST-LIB_LOW/Sensors/DigitalSensor/DigitalSensor.hpp"
#include "DigitalInputService/DigitalInputService.hpp"
#include "EXTI/EXTI.hpp"

DigitalSensor::DigitalSensor(Pin pin, PinState *value) : pin(pin), id(DigitalInput::inscribe(pin)), value(value){}

void DigitalSensor::exti_interruption(std::function<void()> &&action){
	optional<uint8_t> identification = ExternalInterrupt::inscribe(pin, std::move(action));
	if (not identification) {
		//TODO: add Error handler for register here (register returns empty optional)
		return;
	}
	exti_id = identification.value();
	SensorStarter::EXTI_id_list.insert(SensorStarter::EXTI_id_list.begin(),exti_id);
}

void DigitalSensor::read(){
	optional<PinState> val = DigitalInput::read_pin_state(id);
	if(not val){
		//TODO: add Error handler for read here (read returns empty optional)
		return;
	}
	*value = val.value();
}
