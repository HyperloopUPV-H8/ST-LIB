#include "ST-LIB_LOW/Sensors/EncoderSensor/EncoderSensor.hpp"
#include "Encoder/Encoder.hpp"

EncoderSensor::EncoderSensor(Pin pin1, Pin pin2, double counter_distance, double *position, double *speed, double *acceleration)
: counter_distance(counter_distance), position(position), speed(speed), acceleration(acceleration){
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
		*position= value[1] + sign * ((double) counter.value()) * counter_distance;
		*speed = sign * ((double) counter.value()) * counter_distance / (last_check);
		*acceleration = (*speed - last_speed)/last_check;
		Encoder::reset_encoder(id);
		last_check = 1;
		last_speed = *speed;
	}
	else{
		//errores de optional vacio aqui (por hacer)
	}
}

uint8_t EncoderSensor::getID(){
	return id;
}
