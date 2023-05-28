#pragma once
#include "LinearSensor.hpp"
#include "Control/Blocks/MovingAverage.hpp"

template<size_t N>
class FilteredLinearSensor : public LinearSensor{
	MovingAverage<N>& filter;

	FilteredLinearSensor(Pin& pin, double slope, double offset, double* value, MovingAverage<N>& filter) : LinearSensor(pin, slope, offset, value), filter(filter){}
	FilteredLinearSensor(Pin& pin, double slope, double offset, double& value, MovingAverage<N>& filter) : LinearSensor(pin, slope, offset, value), filter(filter){}
	void read() override{
		float val = ADC::get_value(id);
		*value = filter.compute(slope * (double) val + offset);
	}
};

//CTAD
#if __cpp_deduction_guides >= 201606
template<size_t N>
FilteredLinearSensor(Pin& pin, double slope, double offset, double* value, MovingAverage<N>& filter)->FilteredLinearSensor<N>;
template<size_t N>
FilteredLinearSensor(Pin& pin, double slope, double offset, double& value, MovingAverage<N>& filter)->FilteredLinearSensor<N>;
#endif
