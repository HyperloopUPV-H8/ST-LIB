#pragma once
#include "ADC/ADC.hpp"
#include "Sensors/Sensor/Sensor.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "Control/ControlBlock.hpp"
#include "Control/Blocks/MovingAverage.hpp"

class PT100{
public:
	static constexpr float k = 841.836735;
	static constexpr float offset = -492.204082;

	ControlBlock<float,float>* filter = nullptr;

	template<size_t N>
	PT100(Pin& pin, float* value, MovingAverage<N>& filter);
	PT100(Pin& pin, float* value);

	template<size_t N>
	PT100(Pin& pin, float& value, MovingAverage<N>& filter);
	PT100(Pin& pin, float& value);

	void read();
protected:
	uint8_t id;
	float* value;
};

template<size_t N>
PT100::PT100(Pin& pin, float* value, MovingAverage<N>& filter) : value(value), filter(&filter){
	id = ADC::inscribe(pin);
	Sensor::adc_id_list.push_back(id);
}

template<size_t N>
PT100::PT100(Pin& pin, float& value, MovingAverage<N>& filter) : value(&value), filter(&filter){
	id = ADC::inscribe(pin);
	Sensor::adc_id_list.push_back(id);
}

PT100::PT100(Pin& pin, float* value) : value(value){
	id = ADC::inscribe(pin);
	Sensor::adc_id_list.push_back(id);
}

PT100::PT100(Pin& pin, float& value) : value(&value){
	id = ADC::inscribe(pin);
	Sensor::adc_id_list.push_back(id);
}

void PT100::read(){
	float val = ADC::get_value(id);
	if(filter != nullptr){
		filter->input(k/val + offset);
		filter->execute();
		*value = filter->output_value;
	}else *value = k/val + offset;
}
