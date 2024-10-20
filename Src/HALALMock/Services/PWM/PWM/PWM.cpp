#include "HALALMock/Services/PWM/PWM/PWM.hpp"
PWM::PWM(Pin& pin) {
	/*By HalalMock we only will test the logical interface of PWM
	if the PWM works, however when you try in your micro doesn't work,
	you should check the timers and channels, that could be the problem 
	*/
	EmulatedPin &pin_memory=SharedMemory::get_pin(pin);
	if(pin_memory.type==PinType::NOT_USED){
		pin_memory.type=PinType::PWM;
		pin_memory.PinData.PWM.duty_cycle=0.0f;
		pin_memory.PinData.PWM.is_on=false;
		//let's point our class variables to the variables from PinModel
		this->duty_cycle=&(pin_memory.PinData.PWM.duty_cycle);
		this->frequency=&(pin_memory.PinData.PWM.frequency);
		this->is_on=&(pin_memory.PinData.PWM.is_on);
		//default values
		*(this->duty_cycle)=0.0f;
		*(this->is_on)=false;
		*(this->frequency)=0;
	}else{
		ErrorHandler("Pin %s is being used already",pin.to_string());
	}
}

void PWM::turn_on() {
	if(*(this->frequency)==0){
		ErrorHandler("You need to set a frequency before turn_on the PWM :%d",*(this->frequency));
	}else{
		*(this->is_on) = true;
	}
  
}

void PWM::turn_off() {
  *(this->is_on) = false;
}

void PWM::set_duty_cycle(float duty_cycle) {
	*(this->duty_cycle) = duty_cycle;
}

void PWM::set_frequency(uint32_t frequency) {
	*(this->frequency) = frequency;
}

uint32_t PWM::get_frequency() {
	return *(this->frequency);
}

float PWM::get_duty_cycle(){
	return *(this->duty_cycle);
}
void PWM::set_dead_time(std::chrono::nanoseconds dead_time_ns)
{
	if(*(this->is_on)){
		ErrorHandler("%s","This function can not be called if the PWM is on");
	}
	/*
		Code that creates a dead time in the mock where duty_cycle is 0
	*/
	return;

}
