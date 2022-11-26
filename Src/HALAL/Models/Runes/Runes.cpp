/*
 * maps.hpp
 *
 *  Created on: Nov 21, 2022
 *      Author: alejandro
 */

#include "ST-LIB.hpp"

/************************************************
 *                 	  Encoder
 ***********************************************/
#ifdef HAL_TIM_MODULE_ENABLED

extern TIM_HandleTypeDef htim8;

map<pair<Pin, Pin>, TIM_HandleTypeDef*> Encoder::pin_timer_map = {
		{{PC6, PC7}, {&htim8}},
};

#endif

/************************************************
 *                     PWM
 ***********************************************/
#ifdef HAL_TIM_MODULE_ENABLED

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim23;

map<Pin, TimerChannel> PWM::pin_timer_map = {
		{PE6, {&htim15, TIM_CHANNEL_2}},
		{PF1, {&htim23, TIM_CHANNEL_2}}
};

map<Pin, TimerChannel> PWM::pin_timer_map_negated = {};

map<pair<Pin, Pin>, TimerChannel> PWM::pin_timer_map_dual = {
		{{PE4, PE5}, {&htim15, TIM_CHANNEL_1}}
};

#endif

/************************************************
 *                 Input Capture
 ***********************************************/
#ifdef HAL_TIM_MODULE_ENABLED

extern TIM_HandleTypeDef htim2;

map<Pin, InputCapture::Instance> InputCapture::instances_data = {
		{PA0, InputCapture::Instance(PA0, &htim2, TIM_CHANNEL_1, TIM_CHANNEL_2)}
};

#endif


/************************************************
 *					   ADC
 ***********************************************/
#ifdef HAL_ADC_MODULE_ENABLED

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

extern LPTIM_HandleTypeDef hlptim1;
extern LPTIM_HandleTypeDef hlptim2;
extern LPTIM_HandleTypeDef hlptim3;

uint16_t* adc_buf1;
uint16_t* adc_buf2;
uint16_t* adc_buf3;

LowPowerTimer lptim1 = LowPowerTimer(&hlptim1, LPTIM1_PERIOD);
LowPowerTimer lptim2 = LowPowerTimer(&hlptim2, LPTIM2_PERIOD);
LowPowerTimer lptim3 = LowPowerTimer(&hlptim3, LPTIM3_PERIOD);

ADC::InitData init_data1 = ADC::InitData(ADC1, ADC_RESOLUTION_16B, ADC_EXTERNALTRIG_LPTIM1_OUT);
ADC::InitData init_data2 = ADC::InitData(ADC2, ADC_RESOLUTION_16B, ADC_EXTERNALTRIG_LPTIM2_OUT);
ADC::InitData init_data3 = ADC::InitData(ADC3, ADC_RESOLUTION_12B, ADC_EXTERNALTRIG_LPTIM3_OUT);

map<Pin, uint32_t> available_pins1 = {
		{PF11, TIM_CHANNEL_2},
		{PA6, TIM_CHANNEL_3}};
map<Pin, uint32_t> available_pins2 = {};
map<Pin, uint32_t> available_pins3 = {};



ADC::Peripheral adc1 = ADC::Peripheral(&hadc1, adc_buf1, hlptim1, init_data1, available_pins1);
ADC::Peripheral adc2 = ADC::Peripheral(&hadc2, adc_buf2, hlptim2, init_data2, available_pins2);
ADC::Peripheral adc3 = ADC::Peripheral(&hadc3, adc_buf3, hlptim3, init_data3, available_pins3);

#endif

/************************************************
 *					   EXTI
 ***********************************************/
#ifdef HAL_EXTI_MODULE_ENABLED

map<uint16_t, ExternalInterrupt::Instance> ExternalInterrupt::instances = {
	{PE7.gpio_pin, Instance()}
};

#endif

