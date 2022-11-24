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

DMAStream DMAStream1 = DMAStream(adc_buf1);
LowPowerTimer lptim1 = LowPowerTimer(&hlptim1, LPTIM1_PERIOD);

DMAStream DMAStream2 = DMAStream(adc_buf2);
LowPowerTimer lptim2 = LowPowerTimer(&hlptim2, LPTIM2_PERIOD);

DMAStream DMAStream3 = DMAStream(adc_buf3);
LowPowerTimer lptim3 = LowPowerTimer(&hlptim3, LPTIM3_PERIOD);

map<Pin, ADC::Instance> ADC::available_instances = {
		{PF11, Instance(&hadc1, 1, lptim1, DMAStream1)},
		{PA6, Instance(&hadc1, 2, lptim1, DMAStream1)},
		{PC4, Instance(&hadc1, 3, lptim1, DMAStream1)},
		{PB1, Instance(&hadc1, 4, lptim1, DMAStream1)},
		{PF13, Instance(&hadc2, 1, lptim2, DMAStream2)},
		{PF14, Instance(&hadc2, 2, lptim2, DMAStream2)},
		{PC2, Instance(&hadc3, 1, lptim3, DMAStream3)},
		{PC3, Instance(&hadc3, 2, lptim3, DMAStream3)}
};

map<ADC_HandleTypeDef*, ADC::InitData> ADC::init_data_map = {
		{&hadc1, InitData(
			ADC1,
			ADC_RESOLUTION_16B,
			ADC_EXTERNALTRIG_LPTIM1_OUT,
			{{ADC_CHANNEL_2, ADC_REGULAR_RANK_1}, {ADC_CHANNEL_3, ADC_REGULAR_RANK_2}}
		)},
		{&hadc2, InitData(
			ADC2,
			ADC_RESOLUTION_16B,
			ADC_EXTERNALTRIG_LPTIM2_OUT,
			{{ADC_CHANNEL_2, ADC_REGULAR_RANK_1}, {ADC_CHANNEL_3, ADC_REGULAR_RANK_2}}
		)},
		{&hadc3, InitData(
			ADC3,
			ADC_RESOLUTION_12B,
			ADC_EXTERNALTRIG_LPTIM3_OUT,
			{{ADC_CHANNEL_2, ADC_REGULAR_RANK_1}, {ADC_CHANNEL_3, ADC_REGULAR_RANK_2}}
		)}
};

#endif

/************************************************
 *					   EXTI
 ***********************************************/
#ifdef HAL_EXTI_MODULE_ENABLED

map<uint16_t, ExternalInterrupt::Instance> ExternalInterrupt::instances = {
	{PE7.gpio_pin, Instance()}
};

#endif

