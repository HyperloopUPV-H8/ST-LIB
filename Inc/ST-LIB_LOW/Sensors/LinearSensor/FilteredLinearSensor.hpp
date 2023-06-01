#pragma once
#include "LinearSensor.hpp"
#include "Control/Blocks/MovingAverage.hpp"

template<size_t N>
class FilteredLinearSensor : public LinearSensor{
	MovingAverage<N>& filter;
public:

	FilteredLinearSensor(Pin& pin, float slope, float offset, float* value, MovingAverage<N>& filter) : LinearSensor(pin, slope, offset, value), filter(filter){}
	FilteredLinearSensor(Pin& pin, float slope, float offset, float& value, MovingAverage<N>& filter) : LinearSensor(pin, slope, offset, value), filter(filter){}
	void read(){
		float val = ADC::get_value(id);
		*value = filter.compute(slope * (float) val + offset);
	}
};

//CTAD
#if __cpp_deduction_guides >= 201606
template<size_t N>
FilteredLinearSensor(Pin& pin, float slope, float offset, float* value, MovingAverage<N>& filter)->FilteredLinearSensor<N>;
template<size_t N>
FilteredLinearSensor(Pin& pin, float slope, float offset, float& value, MovingAverage<N>& filter)->FilteredLinearSensor<N>;
#endif
