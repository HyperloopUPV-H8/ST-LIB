#include "HALALMock/Services/PWM/PWM/PWM.hpp"
PWM::PWM(Pin& pin) {
	/*By HalalMock we only will test the logical interface of PWM
	if the PWM works, however when you try in your micro doesn't work,
	you should check the timers and channels, that could be the problem 
	*/
	uint8_t offset;
	auto it=pin_offsets.find(pin);
	if(it != pin_offsets.end()){
		offset =it->second;
	}else{
		ErrorHandler("Pin %s doesn't exist",pin.to_string());
	}
	PinModel *pin_memory=(SharedMemory::gpio_memory)+offset;
	if(pin_memory->type==PinType::NOT_USED){
		pin_memory->type=PinType::PWM;
		pin_memory->PinData.PWM.duty_cycle=0.0f;
		pin_memory->PinData.PWM.is_on=false;
		//let's point our class variables to the variables from PinModel
		duty_cycle=&(pin_memory->PinData.PWM.duty_cycle);
		frequency=&(pin_memory->PinData.PWM.frequency);
		is_on=
	}else{
		ErrorHandler("Pin %s is being used already",pin.to_string());
	}
	
	
}

void PWM::turn_on() {
  if(not is_initialized){
	ErrorHandler("PWM was not initialized");
  }
  if(is_on) return;
  if(HAL_TIM_PWM_Start(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("PWM did not turn on correctly", 0);
  }
  is_on = true;
}

void PWM::turn_off() {
  if(not is_on) return;
  if(HAL_TIM_PWM_Stop(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("PWM did not stop correctly", 0);
  }
  is_on = false;
}

void PWM::set_duty_cycle(float duty_cycle) {
	uint16_t raw_duty = __HAL_TIM_GET_AUTORELOAD(peripheral->handle) / 100.0 * duty_cycle;
	__HAL_TIM_SET_COMPARE(peripheral->handle, channel, raw_duty);
	this->duty_cycle = duty_cycle;
}

void PWM::set_frequency(uint32_t frequency) {
	this->frequency = frequency;
	TIM_TypeDef& timer = *peripheral->handle->Instance;
	timer.ARR = (HAL_RCC_GetPCLK1Freq()*2 / (timer.PSC+1)) / frequency;
	set_duty_cycle(duty_cycle);
}

uint32_t PWM::get_frequency() {
	return frequency;
}

float PWM::get_duty_cycle(){
	return duty_cycle;
}
void PWM::set_dead_time(std::chrono::nanoseconds dead_time_ns)
{
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
	
	// per https://hasanyavuz.ozderya.net/?p=437
	auto time = dead_time_ns.count();

	if(time <= 127 * clock_period_ns){
		sBreakDeadTimeConfig.DeadTime = time/clock_period_ns;
	}else if (time >127 * clock_period_ns && time  <= 2 * clock_period_ns * 127)
	{
		sBreakDeadTimeConfig.DeadTime = time /(2 * clock_period_ns) - 64 + 128;
	}else if(time > 2 * clock_period_ns * 127 && time <= 8 * clock_period_ns * 127){
		sBreakDeadTimeConfig.DeadTime = time/(8 * clock_period_ns) -32 + 192;
	}else if(time > 8 * clock_period_ns * 127 && time <=16 * clock_period_ns*127){
		sBreakDeadTimeConfig.DeadTime = time/(16 * clock_period_ns) -32 + 224;
	}else{
		ErrorHandler("Invalid dead time configuration");
	}

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(peripheral->handle,&sBreakDeadTimeConfig);
	peripheral->handle->Instance->BDTR |= TIM_BDTR_MOE;

	return;

}
