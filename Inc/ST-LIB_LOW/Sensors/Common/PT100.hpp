#pragma once
#include "ADC/ADC.hpp"
#include "Sensors/Sensor/Sensor.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "Control/ControlBlock.hpp"
#include "Control/Blocks/MovingAverage.hpp"

<<<<<<< Updated upstream
=======
template<size_t N>
>>>>>>> Stashed changes
class PT100{
public:
	static constexpr float k = 841.836735;
	static constexpr float offset = -492.204082;

<<<<<<< Updated upstream
	ControlBlock<float,float>* filter = nullptr;

	template<size_t N>
	PT100(Pin& pin, float* value, MovingAverage<N>& filter);
	PT100(Pin& pin, float* value);

	template<size_t N>
=======
	MovingAverage<N>* filter = nullptr;

	PT100(Pin& pin, float* value, MovingAverage<N>& filter);
	PT100(Pin& pin, float* value);

>>>>>>> Stashed changes
	PT100(Pin& pin, float& value, MovingAverage<N>& filter);
	PT100(Pin& pin, float& value);

	void read();
protected:
	uint8_t id;
	float* value;
};

template<size_t N>
<<<<<<< Updated upstream
PT100::PT100(Pin& pin, float* value, MovingAverage<N>& filter) : value(value), filter(&filter){
=======
PT100<N>::PT100(Pin& pin, float* value, MovingAverage<N>& filter) : value(value), filter(&filter){
>>>>>>> Stashed changes
	id = ADC::inscribe(pin);
	Sensor::adc_id_list.push_back(id);
}

template<size_t N>
<<<<<<< Updated upstream
PT100::PT100(Pin& pin, float& value, MovingAverage<N>& filter) : value(&value), filter(&filter){
=======
PT100<N>::PT100(Pin& pin, float& value, MovingAverage<N>& filter) : value(&value), filter(&filter){
>>>>>>> Stashed changes
	id = ADC::inscribe(pin);
	Sensor::adc_id_list.push_back(id);
}

<<<<<<< Updated upstream
PT100::PT100(Pin& pin, float* value) : value(value){
=======
template<size_t N>
PT100<N>::PT100(Pin& pin, float* value) : value(value){
>>>>>>> Stashed changes
	id = ADC::inscribe(pin);
	Sensor::adc_id_list.push_back(id);
}

<<<<<<< Updated upstream
PT100::PT100(Pin& pin, float& value) : value(&value){
=======
template<size_t N>
PT100<N>::PT100(Pin& pin, float& value) : value(&value){
>>>>>>> Stashed changes
	id = ADC::inscribe(pin);
	Sensor::adc_id_list.push_back(id);
}

<<<<<<< Updated upstream
void PT100::read(){
=======
template<size_t N>
void PT100<N>::read(){
>>>>>>> Stashed changes
	float val = ADC::get_value(id);
	if(filter != nullptr){
		filter->input(k/val + offset);
		filter->execute();
		*value = filter->output_value;
	}else *value = k/val + offset;
}
