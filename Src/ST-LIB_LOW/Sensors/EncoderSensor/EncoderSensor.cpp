#include "ST-LIB_LOW/Sensors/EncoderSensor/EncoderSensor.hpp"
#include "Encoder/Encoder.hpp"
#include "Time/Time.hpp"

EncoderSensor::EncoderSensor(Pin pin1, Pin pin2, double counter_distance, double *position, double *speed, double *acceleration)
: counter_distance(counter_distance), position(position), speed(speed), acceleration(acceleration){
	optional<uint8_t> identification = Encoder::register_encoder(pin1,pin2);
	if(identification){
		id = identification.value();
		Encoder::turn_on_encoder(id);
		last_check = Time::get_global_tick();
		last_position = *position;
		last_speed = *speed;
	}else{
		//TODO: Error handler, Error during register of encoder x
	}
}

void EncoderSensor::read(){
	optional<uint32_t> counter = Encoder::get_encoder_counter(id);
	optional<bool> direction = Encoder::get_encoder_direction(id);
	uint64_t check = Time::get_global_tick();

	if(counter && direction){

		int sign = (1 - 2*direction.value());

		*position= sign * ((double) counter.value()) * counter_distance;
		*speed = (*position - last_position) * clock_frequency / (check - last_check);
		*acceleration = (*speed - last_speed) * clock_frequency/ (check - last_check);

		last_check = check;
		last_position = *position;
		last_speed = *speed;
	}
	else{
		//errores de optional vacio aqui (por hacer)
	}
}

uint8_t EncoderSensor::getID(){
	return id;
}
