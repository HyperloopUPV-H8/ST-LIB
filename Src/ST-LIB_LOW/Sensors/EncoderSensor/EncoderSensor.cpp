#include "ST-LIB_LOW/Sensors/EncoderSensor/EncoderSensor.hpp"
#include "Encoder/Encoder.hpp"

EncoderSensor::EncoderSensor(Pin pin1, Pin pin2, double* value, double counter_distance)
: value(value), counter_distance(counter_distance){
	optional<uint8_t> identification = Encoder::register_encoder(pin1,pin2);
	if(identification){
		id = identification.value();
		Encoder::turn_on_encoder(id);
		last_check = 1;
	}else{
		//errores de optional vacio aqui (por hacer)
	}
}

void EncoderSensor::read(){
	optional<uint32_t> counter = Encoder::get_encoder_counter(id);
	optional<bool> direction = Encoder::get_encoder_direction(id);
	if(counter && direction){
		//calcular valores aqui [0] velocidad y [1] posicion
		int sign = (1 - 2*direction.value());
		value[0] = sign * ((double) counter.value()) * counter_distance / (last_check);
		value[1] = value[1] + sign * ((double) counter.value()) * counter_distance;
		Encoder::reset_encoder(id);
	}
	else{
		//errores de optional vacio aqui (por hacer)
	}
}

uint8_t EncoderSensor::getID(){
	return id;
}
