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
 *                     PWMservice
 ***********************************************/
#ifdef HAL_TIM_MODULE_ENABLED

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim23;

PWMservice::TimerInitData init_data_timer1 = PWMservice::TimerInitData(TIM1, GPIOE, 2750, 1000);
PWMservice::TimerInitData init_data_timer15 = PWMservice::TimerInitData(TIM15, GPIOE, 0, 65535);


PWMservice::TimerPeripheral PWMservice::timer_peripherals[H723_TIMERS] = {
		TimerPeripheral(&htim1, init_data_timer1),
		TimerPeripheral(&htim15, init_data_timer15)};
map<Pin, PWMservice::Instance> PWMservice::available_instances = {
		{PE14, PWMservice::Instance(&timer_peripherals[0], TIM_CHANNEL_4, NORMAL)}
};

map<Pin, PWMservice::Instance> PWMservice::available_instances_negated = {};

map<pair<Pin, Pin>, PWMservice::Instance> PWMservice::available_instances_dual = {
		{{PE9, PE8}, PWMservice::Instance(&timer_peripherals[1], TIM_CHANNEL_1, DUAL)},
		{{PE11, PE10}, PWMservice::Instance(&timer_peripherals[1], TIM_CHANNEL_2, DUAL)},
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
#ifdef HAL_ADC_MODULE_ENABLED && HAL_LPTIM_MODULE_ENABLED

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

extern LPTIM_HandleTypeDef hlptim1;
extern LPTIM_HandleTypeDef hlptim2;
extern LPTIM_HandleTypeDef hlptim3;

uint16_t adc_buf1[ADC_BUF_LEN];
uint16_t adc_buf2[ADC_BUF_LEN];
uint16_t adc_buf3[ADC_BUF_LEN];

LowPowerTimer lptim1 = LowPowerTimer(&hlptim1, LPTIM1_PERIOD);
LowPowerTimer lptim2 = LowPowerTimer(&hlptim2, LPTIM2_PERIOD);
LowPowerTimer lptim3 = LowPowerTimer(&hlptim3, LPTIM3_PERIOD);

vector<uint32_t> channels1 = {};
vector<uint32_t> channels2 = {};
vector<uint32_t> channels3 = {};

ADC::InitData init_data1 = ADC::InitData(ADC1, ADC_RESOLUTION_16B, ADC_EXTERNALTRIG_LPTIM1_OUT, channels1);
ADC::InitData init_data2 = ADC::InitData(ADC2, ADC_RESOLUTION_16B, ADC_EXTERNALTRIG_LPTIM2_OUT, channels2);
ADC::InitData init_data3 = ADC::InitData(ADC3, ADC_RESOLUTION_12B, ADC_EXTERNALTRIG_LPTIM3_OUT, channels3);

ADC::Peripheral ADC::peripherals[3] = {
		ADC::Peripheral(&hadc1, adc_buf1, lptim1, init_data1),
		ADC::Peripheral(&hadc2, adc_buf2, lptim2, init_data2),
		ADC::Peripheral(&hadc3, adc_buf3, lptim3, init_data3)
};

map<Pin, ADC::Instance> ADC::available_instances = {
		{PF11, Instance(&peripherals[0], ADC_CHANNEL_2)},
		{PA6, Instance(&peripherals[0], ADC_CHANNEL_3)}
};

uint32_t ADC::ranks[16] = {
		ADC_REGULAR_RANK_1,
		ADC_REGULAR_RANK_2,
		ADC_REGULAR_RANK_3,
		ADC_REGULAR_RANK_4,
		ADC_REGULAR_RANK_5,
		ADC_REGULAR_RANK_6,
		ADC_REGULAR_RANK_7,
		ADC_REGULAR_RANK_8,
		ADC_REGULAR_RANK_9,
		ADC_REGULAR_RANK_10,
		ADC_REGULAR_RANK_11,
		ADC_REGULAR_RANK_12,
		ADC_REGULAR_RANK_13,
		ADC_REGULAR_RANK_14,
		ADC_REGULAR_RANK_15,
		ADC_REGULAR_RANK_16
};



#endif

/************************************************
 *					   EXTI
 ***********************************************/
#ifdef HAL_EXTI_MODULE_ENABLED

map<uint16_t, ExternalInterrupt::Instance> ExternalInterrupt::instances = {
	{PE0.gpio_pin, Instance(EXTI0_IRQn)}
};

#endif

