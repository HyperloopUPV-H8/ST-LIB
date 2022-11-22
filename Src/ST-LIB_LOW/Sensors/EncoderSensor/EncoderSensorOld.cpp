#include "ST-LIB_LOW/Sensors/EncoderSensor/EncoderSensor.hpp"
#include "Encoder/Encoder.hpp"
#include "Time/Time.hpp"

EncoderSensor::EncoderSensor(Pin pin1, Pin pin2, double *position, double *speed, double *acceleration)
: position(position), speed(speed), acceleration(acceleration){
	optional<uint8_t> identification = Encoder::register_encoder(pin1,pin2);
	if(identification){
		id = identification.value();
		Encoder::turn_on_encoder(id);
		last_time = Time::get_global_tick();
		last_position = *position;
		last_speed = *speed;
	}else{
		id = 0xFFFF;
		//TODO: Error handler,Error during register of encoder x
	}
}

void EncoderSensor::read(){
	optional<uint32_t> counter = Encoder::get_encoder_counter(id);
	optional<bool> direction = Encoder::get_encoder_direction(id);
	uint64_t delta_time = Time::get_global_tick() - last_time;
	
	if(counter && direction){

		*position= ((double) counter.value()) * counter_distance;
		*speed = (*position - last_position) * clock_frequency / (delta_time);
		*acceleration = (*speed - last_speed) * clock_frequency/ (delta_time);

		last_time += delta_time;
		last_position = *position;
		last_speed = *speed;
	}
	else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

uint8_t EncoderSensor::getID(){
	return id;
}
