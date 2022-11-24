#include "ST-LIB_LOW/Sensors/EncoderSensor/EncoderSensor.hpp"
#include "Encoder/Encoder.hpp"
#include "Time/Time.hpp"

EncoderSensor::EncoderSensor(Pin pin1, Pin pin2, double *position, double *speed, double *acceleration)
: position(position), speed(speed), acceleration(acceleration){
	optional<uint8_t> identification = Encoder::register_encoder(pin1,pin2);
	if(identification){
		id = identification.value();
	}else{
		//TODO: Error handler, Error during register of encoder x
		id = 69;
	}
}

void EncoderSensor::start(){
	Encoder::reset_encoder(id);
	Encoder::turn_on_encoder(id);
	for(int i = 0; i < n_frames; i++){
		counters[i] = 0;
		times[i] = Time::get_global_tick();;
	}
	last_speed = *speed;
}

void EncoderSensor::read(){
	optional<uint32_t> optional_counter = Encoder::get_encoder_counter(id);
	optional<bool> optional_direction = Encoder::get_encoder_direction(id);
	uint64_t time = Time::get_global_tick();
	
	if(optional_counter && optional_direction){
		uint32_t counter = optional_counter.value();

		if(time - times[n_frames-1] >= frame_size*clock_frequency){
			for(int i = 1; i < n_frames; i++){
				counters[i-1] = counters[i];
				times[i-1] = times[i];
			}

			if(counter - counters[n_frames-1] > 0xEFFFFFFF){
				counter = ~counter;
			}

			counters[n_frames-1] = counter;
			times[n_frames-1] = time;
			last_speed = *speed;
		}

		int delta_time = times[n_frames-1] - times[0];
		double last_position = ((int) counters[0]) * counter_distance;

		*position= ((int) counters[n_frames-1]) * counter_distance;

		*speed = abs(*position - last_position) * clock_frequency / (delta_time); //Usa n_frames frames para el calculo

		*acceleration = (*speed - last_speed) * clock_frequency * (n_frames-1) / (delta_time); //usa solo 1 frame (y el anterior) para el calculo
	}
	else{
		//TODO: empty optional error handle 
	}
}

uint8_t EncoderSensor::get_id(){
	return id;
}
