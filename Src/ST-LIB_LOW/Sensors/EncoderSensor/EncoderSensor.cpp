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
		id = 69;
	}
}

void EncoderSensor::start(){
	Encoder::reset_encoder(id);
	Encoder::turn_on_encoder(id);
	uint64_t clock_time = Time::get_global_tick();
	for(int i = 0; i < N_FRAMES; i++){
		positions[i] = 0.0;
		times[i] = i*FRAME_SIZE - N_FRAMES*FRAME_SIZE + (((int) clock_time)/ CLOCK_FREQUENCY);
		speeds[i] = 0.0;
	}
	time = 0.0;
	last_clock_time = clock_time;
}

void EncoderSensor::read(){
	optional<uint32_t> optional_counter = Encoder::get_encoder_counter(id);
	optional<bool> optional_direction = Encoder::get_encoder_direction(id);
	uint64_t clock_time = Time::get_global_tick();
	
	if(optional_counter && optional_direction){
		if(clock_time < last_clock_time){last_clock_time = clock_time;}
		time = time + ((int)clock_time - (int)last_clock_time) / CLOCK_FREQUENCY;

		*position= ((int) optional_counter.value() - START_COUNTER) * COUNTER_DISTANCE;
		double delta_time = time - times[0];
		double delta_position = *position - positions[0];

		*speed = abs(delta_position) / (delta_time);
		double delta_speed = *speed - speeds[0];

		*acceleration = (delta_speed) / (delta_time);

		if(time - times[N_FRAMES-1] >= FRAME_SIZE){EncoderSensor::update_arrays();}
		last_clock_time = clock_time;
	}
	else{
		//TODO: add Error handler for read here (read returns empty optional)
	}
}

uint8_t EncoderSensor::get_id(){
	return id;
}


double** EncoderSensor::get_arrays(){
	double **ret = new double*[3];
	for(int n = 0; n < 3; n++){
		ret[n] = new double[N_FRAMES];
		for(int i = 0; i < N_FRAMES; i++){
			if(n == 0)
				ret[n][i] = positions[i];
			if(n == 1)
				ret[n][i] = times[i];
			if(n == 2)
				ret[n][i] = speeds[i];
		}
	}
	return ret;
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
