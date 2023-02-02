#include "Sensors/SensorInterrupt/SensorInterrupt.hpp"
#include "Sensors/Sensor/Sensor.hpp"

SensorInterrupt::SensorInterrupt(Pin &pin, std::function<void()> &&action, PinState *value) : pin(pin), value(value) {
	optional<uint8_t> identification = ExternalInterrupt::inscribe(pin, std::forward<std::function<void()>>(action));
	if (not identification) {
		ErrorHandler(" The pin %s is already used or isn t available for EXTI usage", pin.to_string().c_str());
		return;
	}
	id = identification.value();
	Sensor::EXTI_id_list.insert(Sensor::EXTI_id_list.begin(),id);
}

SensorInterrupt::SensorInterrupt(Pin &pin, std::function<void()> &&action, PinState &value) : SensorInterrupt::SensorInterrupt(pin,std::forward<std::function<void()>>(action),&value){}

void SensorInterrupt::read(){
	optional<bool> optional_value = ExternalInterrupt::get_pin_value(id);
	if(not optional_value){
		ErrorHandler("Could not read state of EXTI pin %s", pin.to_string().c_str());
	}else if(optional_value.value()){
		*value = ON;
	}else{
		*value = OFF;
	}
}

uint8_t SensorInterrupt::get_id(){
	return id;
}
