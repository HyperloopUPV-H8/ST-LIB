#include "ST-LIB_LOW/Sensors/EncoderSensor/EncoderSensor.hpp"
#include "Encoder/Encoder.hpp"
#include "Time/Time.hpp"

const int n_frames = 5; //debe ser >1, cuanto mas alto mayor suavizado y mayor coste de computacion
const double frame_size = 0.01; //sacrificar tiempo de refresco por minima velocidad detectable (mas alto, mejor precision (minimo baja), mas bajo mejor tiempo de refresco)
uint32_t counters[n_frames]; //minima velocidad detectable es igual a counter_distance * n_frames / frame_size
uint64_t times[n_frames];

EncoderSensor::EncoderSensor(Pin pin1, Pin pin2, double *position, double *speed, double *acceleration)
: position(position), speed(speed), acceleration(acceleration){
	optional<uint8_t> identification = Encoder::register_encoder(pin1,pin2);
	if(identification){
		id = identification.value();
		Encoder::turn_on_encoder(id);
		for(int i = 0; i < n_frames; i++){
						counters[i] = 0;
						times[i] = Time::get_global_tick();;
					}
		last_position = *position;
		last_speed = *speed;
	}else{
		//TODO: Error handler, Error during register of encoder x
	}
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
		last_position = ((double) counters[0]) * counter_distance;

		*position= ((double) counters[n_frames-1]) * counter_distance;

		*speed = abs(*position - last_position) * clock_frequency / (delta_time); //Usa n_frames frames para el calculo

		*acceleration = (*speed - last_speed) * clock_frequency * (n_frames-1) / (delta_time); //usa solo 1 frame (y el anterior) para el calculo
	}
	else{
		//TODO: errores de optional vacio aqui (por hacer)
	}
}

uint8_t EncoderSensor::getID(){
	return id;
}
