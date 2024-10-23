#include "HALALMock/Services/PWM/DualPWM/DualPWM.hpp"

DualPWM::DualPWM(Pin& pin_pos, Pin& pin_neg) {
	EmulatedPin &pin_positive = SharedMemory::get_pin(pin_pos);
	EmulatedPin &pin_negative = SharedMemory::get_pin(pin_neg);

	if(pin_positive.type==PinType::NOT_USED && pin_negative.type==PinType::NOT_USED){
		if(pin_positive.type==PinType::NOT_USED)
		{
			pin_positive.type=PinType::DualPWM;
			// for common values between positive and negative pin, class variables
			// point to the positive pin memory location, then the negative pin 
			// copies the value
			this->duty_cycle=&(pin_positive.PinData.DualPWM.duty_cycle);
			this->frequency=&(pin_positive.PinData.DualPWM.frequency);
			positive_is_on=&(pin_positive.PinData.DualPWM.is_on);
			this->dead_time_ns=&(pin_positive.PinData.DualPWM.dead_time_ns);
		}
		if(pin_negative.type==PinType::NOT_USED)
		{
			pin_negative.type=PinType::DualPWM;
			negative_is_on=&(pin_negative.PinData.DualPWM.is_on);
		}
	}else{
		ErrorHandler("Pin %s or %s is already in use", pin_pos.to_string(), pin_neg.to_string());
	}

	//default values

	*(this->duty_cycle)=0.0f;
	pin_negative.PinData.DualPWM.duty_cycle=*(this->duty_cycle);
	*(this->frequency)=0;
	pin_negative.PinData.DualPWM.frequency=*(this->frequency);
	*positive_is_on=false;
	*negative_is_on=false;
	*(this->dead_time_ns)=0;
	pin_negative.PinData.DualPWM.dead_time_ns=*(this->dead_time_ns);
}


void DualPWM::turn_on() {
	*positive_is_on=true;
	*negative_is_on=true;
}

void DualPWM::turn_off() {
	*positive_is_on=false;
	*negative_is_on=false;
}

void DualPWM::turn_on_positive() {
	*positive_is_on=true;
	*negative_is_on=false;
}

void DualPWM::turn_on_negated() {
  	*positive_is_on=false;
	*negative_is_on=true;
}

void DualPWM::turn_off_positive() {
  	*positive_is_on=false;
}

void DualPWM::turn_off_negated() {
	*negative_is_on=false;
}

void DualPWM::set_duty_cycle(float dc){
	*(this->duty_cycle) = dc;
}
void DualPWM::set_frequency(uint32_t freq){
  	*(this->frequency) = freq;
}
uint32_t DualPWM::get_frequency()const{
  return *(this->frequency);
}
float DualPWM::get_duty_cycle()const{
  return *(this->duty_cycle);
}
void DualPWM::set_dead_time(std::chrono::nanoseconds dead_t_ns)
{
	*(this->dead_time_ns)=dead_t_ns;
	if(*positive_is_on || *negative_is_on){
		ErrorHandler("%s","This function can not be called if the PWM is on");
	}
	/*
		Code that creates a dead time in the mock where duty_cycle is 0
	*/
	return;

}

