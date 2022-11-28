#include "ST-LIB_LOW/Sensors/EncoderSensor/EncoderSensor.hpp"
#include "Encoder/Encoder.hpp"
#include "Time/Time.hpp"

EncoderSensor::EncoderSensor(Pin pin1, Pin pin2, double *position, double *speed, double *acceleration)
: position(position), speed(speed), acceleration(acceleration){
	optional<uint8_t> identification = Encoder::register_encoder(pin1,pin2);
	if(identification){
		id = identification.value();
	}else{
		//TODO: add Error handler for register here (register returns empty optional)
	}
}

void EncoderSensor::start(){
	Encoder::reset_encoder(id);
	Encoder::turn_on_encoder(id);
	uint64_t clock_time = Time::get_global_tick();
	for(int i = 0; i < N_FRAMES; i++){
		positions[i] = 0.0;
		times[i] = i*FRAME_SIZE_IN_SECONDS - N_FRAMES*FRAME_SIZE_IN_SECONDS + (((int) clock_time)/ NANO_SECOND);
		speeds[i] = 0.0;
	}
	time = 0.0;
	last_clock_time = clock_time;
	clock_frequency = HAL_RCC_GetPCLK1Freq()*2; //TODO: Abstract this out of the HAL (HALAL method needed)
}

void EncoderSensor::read(){
	optional<uint32_t> optional_counter = Encoder::get_encoder_counter(id);
	optional<bool> direction = Encoder::get_encoder_direction(id);
	uint64_t clock_time = Time::get_global_tick();
	
	if(optional_counter && direction){
		long int delta_clock = clock_time - last_clock_time;
		if(clock_time < last_clock_time){ //overflow handle
			delta_clock = clock_time + CLOCK_MAX_VALUE * NANO_SECOND / clock_frequency - last_clock_time;
		}
		time = time + delta_clock / NANO_SECOND;
		last_clock_time = clock_time;

		*position= ((int) optional_counter.value() - START_COUNTER) * COUNTER_DISTANCE_IN_METERS;
		double delta_time = time - times[0];
		double delta_position = *position - positions[0];

		*speed = abs(delta_position) / (delta_time);
		double delta_speed = *speed - speeds[0];

		*acceleration = (delta_speed) / (delta_time);

		if(time - times[N_FRAMES-1] >= FRAME_SIZE_IN_SECONDS){EncoderSensor::update_arrays();}
	}
	else{
		//TODO: add Error handler for read here (read returns empty optional)
	}
}

uint8_t EncoderSensor::get_id(){
	return id;
}


void EncoderSensor::get_arrays(double ret[][N_FRAMES]){
	for(int n = 0; n < 3; n++){
		for(int i = 0; i < N_FRAMES; i++){
			switch(n){
				case 0:
					ret[n][i] = positions[i];
					break;
				case 1:
					ret[n][i] = times[i];
					break;
				case 2:
					ret[n][i] = speeds[i];
					break;
			
			}
		}
	}
}

void EncoderSensor::update_arrays(){
	for(int i = 1; i < N_FRAMES; i++){
			positions[i-1] = positions[i];
			times[i-1] = times[i];
			speeds[i-1] = speeds[i];
		}
	positions[N_FRAMES-1] = *position;
	times[N_FRAMES-1] = time;
	speeds[N_FRAMES-1] = *speed;
}
