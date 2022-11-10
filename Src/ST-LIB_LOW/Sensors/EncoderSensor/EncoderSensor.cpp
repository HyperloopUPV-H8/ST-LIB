#include "ST-LIB_LOW/Sensors/EncoderSensor/EncoderSensor.hpp"
#include "Encoder/Encoder.hpp"

//I have to get the pin's address as register_encoder works with pin pointers.
//quick note, all other services use references but ADC, which uses the value itself.
EncoderSensor::EncoderSensor(Pin pin1, Pin pin2, double* value)
: value(value){
	optional<uint8_t> identification = Encoder::register_encoder(&pin1,&pin2);
	if(identification){
		id = identification.value();
		Encoder::turn_on_encoder(id);
	}else{
		//errores de optional vacio aqui (por hacer)
	}
}

void EncoderSensor::read(){
	optional<long unsigned int> counter = Encoder::get_encoder_counter(id);
	optional<bool> direction = Encoder::get_encoder_direction(id);
	if(counter && direction){
		//calcular valores aqui (por hacer)
		*value = (double) counter.value();
		Encoder::reset_encoder(id);
	}
	else{
		//errores de optional vacio aqui (por hacer)
	}
}

uint8_t EncoderSensor::getID(){
	return id;
}
