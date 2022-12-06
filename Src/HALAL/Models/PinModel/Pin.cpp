/*
 * Pin.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: stefan
 */

#include "PinModel/Pin.hpp"

Pin::Pin(){}

Pin::Pin(GPIO_Port port, GPIO_Pin gpio_pin) : port((GPIO_TypeDef*)port), gpio_pin(gpio_pin){}

Pin PE2(GPIO_Port::PORT_E,GPIO_Pin::PIN_2);
Pin PE3(GPIO_Port::PORT_E,GPIO_Pin::PIN_3);
Pin PE4(GPIO_Port::PORT_E,GPIO_Pin::PIN_4);
Pin PE5(GPIO_Port::PORT_E,GPIO_Pin::PIN_5);
Pin PE6(GPIO_Port::PORT_E,GPIO_Pin::PIN_6);
Pin PC13(GPIO_Port::PORT_C,GPIO_Pin::PIN_13);
Pin PC14(GPIO_Port::PORT_C,GPIO_Pin::PIN_14);
Pin PC15(GPIO_Port::PORT_C,GPIO_Pin::PIN_15);
Pin PF0(GPIO_Port::PORT_F,GPIO_Pin::PIN_0);
Pin PF1(GPIO_Port::PORT_F,GPIO_Pin::PIN_1);
Pin PF2(GPIO_Port::PORT_F,GPIO_Pin::PIN_2);
Pin PF3(GPIO_Port::PORT_F,GPIO_Pin::PIN_3);
Pin PF4(GPIO_Port::PORT_F,GPIO_Pin::PIN_4);
Pin PF5(GPIO_Port::PORT_F,GPIO_Pin::PIN_5);
Pin PF6(GPIO_Port::PORT_F,GPIO_Pin::PIN_6);
Pin PF7(GPIO_Port::PORT_F,GPIO_Pin::PIN_7);
Pin PF8(GPIO_Port::PORT_F,GPIO_Pin::PIN_8);
Pin PF9(GPIO_Port::PORT_F,GPIO_Pin::PIN_9);
Pin PF10(GPIO_Port::PORT_F,GPIO_Pin::PIN_10);
Pin PH0(GPIO_Port::PORT_H,GPIO_Pin::PIN_0);
Pin PH1(GPIO_Port::PORT_H,GPIO_Pin::PIN_1);
Pin PC0(GPIO_Port::PORT_C,GPIO_Pin::PIN_0);
Pin PC1(GPIO_Port::PORT_C,GPIO_Pin::PIN_1);
Pin PC2(GPIO_Port::PORT_C,GPIO_Pin::PIN_2);
Pin PC3(GPIO_Port::PORT_C,GPIO_Pin::PIN_3);
Pin PA0(GPIO_Port::PORT_A,GPIO_Pin::PIN_0);
Pin PA1(GPIO_Port::PORT_A,GPIO_Pin::PIN_1);
Pin PA2(GPIO_Port::PORT_A,GPIO_Pin::PIN_2);
Pin PA3(GPIO_Port::PORT_A,GPIO_Pin::PIN_3);
Pin PA4(GPIO_Port::PORT_A,GPIO_Pin::PIN_4);
Pin PA5(GPIO_Port::PORT_A,GPIO_Pin::PIN_5);
Pin PA6(GPIO_Port::PORT_A,GPIO_Pin::PIN_6);
Pin PA7(GPIO_Port::PORT_A,GPIO_Pin::PIN_7);
Pin PC4(GPIO_Port::PORT_C,GPIO_Pin::PIN_4);
Pin PC5(GPIO_Port::PORT_C,GPIO_Pin::PIN_5);
Pin PB0(GPIO_Port::PORT_B,GPIO_Pin::PIN_0);
Pin PB1(GPIO_Port::PORT_B,GPIO_Pin::PIN_1);
Pin PB2(GPIO_Port::PORT_B,GPIO_Pin::PIN_2);
Pin PF11(GPIO_Port::PORT_F,GPIO_Pin::PIN_11);
Pin PF12(GPIO_Port::PORT_F,GPIO_Pin::PIN_12);
Pin PF13(GPIO_Port::PORT_F,GPIO_Pin::PIN_13);
Pin PF14(GPIO_Port::PORT_F,GPIO_Pin::PIN_14);
Pin PF15(GPIO_Port::PORT_F,GPIO_Pin::PIN_15);
Pin PG0(GPIO_Port::PORT_G,GPIO_Pin::PIN_0);
Pin PG1(GPIO_Port::PORT_G,GPIO_Pin::PIN_1);
Pin PE7(GPIO_Port::PORT_E,GPIO_Pin::PIN_7);
Pin PE8(GPIO_Port::PORT_E,GPIO_Pin::PIN_8);
Pin PE9(GPIO_Port::PORT_E,GPIO_Pin::PIN_9);
Pin PE10(GPIO_Port::PORT_E,GPIO_Pin::PIN_10);
Pin PE11(GPIO_Port::PORT_E,GPIO_Pin::PIN_11);
Pin PE12(GPIO_Port::PORT_E,GPIO_Pin::PIN_12);
Pin PE13(GPIO_Port::PORT_E,GPIO_Pin::PIN_13);
Pin PE14(GPIO_Port::PORT_E,GPIO_Pin::PIN_14);
Pin PE15(GPIO_Port::PORT_E,GPIO_Pin::PIN_15);
Pin PB10(GPIO_Port::PORT_B,GPIO_Pin::PIN_10);
Pin PB11(GPIO_Port::PORT_B,GPIO_Pin::PIN_11);
Pin PB12(GPIO_Port::PORT_B,GPIO_Pin::PIN_12);
Pin PB13(GPIO_Port::PORT_B,GPIO_Pin::PIN_13);
Pin PB14(GPIO_Port::PORT_B,GPIO_Pin::PIN_14);
Pin PB15(GPIO_Port::PORT_B,GPIO_Pin::PIN_15);
Pin PD8(GPIO_Port::PORT_D,GPIO_Pin::PIN_8);
Pin PD9(GPIO_Port::PORT_D,GPIO_Pin::PIN_9);
Pin PD10(GPIO_Port::PORT_D,GPIO_Pin::PIN_10);
Pin PD11(GPIO_Port::PORT_D,GPIO_Pin::PIN_11);
Pin PD12(GPIO_Port::PORT_D,GPIO_Pin::PIN_12);
Pin PD13(GPIO_Port::PORT_D,GPIO_Pin::PIN_13);
Pin PD14(GPIO_Port::PORT_D,GPIO_Pin::PIN_14);
Pin PD15(GPIO_Port::PORT_D,GPIO_Pin::PIN_15);
Pin PG2(GPIO_Port::PORT_G,GPIO_Pin::PIN_2);
Pin PG3(GPIO_Port::PORT_G,GPIO_Pin::PIN_3);
Pin PG4(GPIO_Port::PORT_G,GPIO_Pin::PIN_4);
Pin PG5(GPIO_Port::PORT_G,GPIO_Pin::PIN_5);
Pin PG6(GPIO_Port::PORT_G,GPIO_Pin::PIN_6);
Pin PG7(GPIO_Port::PORT_G,GPIO_Pin::PIN_7);
Pin PG8(GPIO_Port::PORT_G,GPIO_Pin::PIN_8);
Pin PC6(GPIO_Port::PORT_C,GPIO_Pin::PIN_6);
Pin PC7(GPIO_Port::PORT_C,GPIO_Pin::PIN_7);
Pin PC8(GPIO_Port::PORT_C,GPIO_Pin::PIN_8);
Pin PC9(GPIO_Port::PORT_C,GPIO_Pin::PIN_9);
Pin PA8(GPIO_Port::PORT_A,GPIO_Pin::PIN_8);
Pin PA9(GPIO_Port::PORT_A,GPIO_Pin::PIN_9);
Pin PA10(GPIO_Port::PORT_A,GPIO_Pin::PIN_10);
Pin PA11(GPIO_Port::PORT_A,GPIO_Pin::PIN_11);
Pin PA12(GPIO_Port::PORT_A,GPIO_Pin::PIN_12);
Pin PC10(GPIO_Port::PORT_C,GPIO_Pin::PIN_10);
Pin PC11(GPIO_Port::PORT_C,GPIO_Pin::PIN_11);
Pin PC12(GPIO_Port::PORT_C,GPIO_Pin::PIN_12);
Pin PD0(GPIO_Port::PORT_D,GPIO_Pin::PIN_0);
Pin PD1(GPIO_Port::PORT_D,GPIO_Pin::PIN_1);
Pin PD2(GPIO_Port::PORT_D,GPIO_Pin::PIN_2);
Pin PD3(GPIO_Port::PORT_D,GPIO_Pin::PIN_3);
Pin PD4(GPIO_Port::PORT_D,GPIO_Pin::PIN_4);
Pin PD5(GPIO_Port::PORT_D,GPIO_Pin::PIN_5);
Pin PD6(GPIO_Port::PORT_D,GPIO_Pin::PIN_6);
Pin PD7(GPIO_Port::PORT_D,GPIO_Pin::PIN_7);
Pin PG9(GPIO_Port::PORT_G,GPIO_Pin::PIN_9);
Pin PG10(GPIO_Port::PORT_G,GPIO_Pin::PIN_10);
Pin PG11(GPIO_Port::PORT_G,GPIO_Pin::PIN_11);
Pin PG12(GPIO_Port::PORT_G,GPIO_Pin::PIN_12);
Pin PG13(GPIO_Port::PORT_G,GPIO_Pin::PIN_13);
Pin PG14(GPIO_Port::PORT_G,GPIO_Pin::PIN_14);
Pin PG15(GPIO_Port::PORT_G,GPIO_Pin::PIN_15);
Pin PB4(GPIO_Port::PORT_B,GPIO_Pin::PIN_4);
Pin PB5(GPIO_Port::PORT_B,GPIO_Pin::PIN_5);
Pin PB6(GPIO_Port::PORT_B,GPIO_Pin::PIN_6);
Pin PB7(GPIO_Port::PORT_B,GPIO_Pin::PIN_7);
Pin PB8(GPIO_Port::PORT_B,GPIO_Pin::PIN_8);
Pin PB9(GPIO_Port::PORT_B,GPIO_Pin::PIN_9);
Pin PE0(GPIO_Port::PORT_E,GPIO_Pin::PIN_0);
Pin PE1(GPIO_Port::PORT_E,GPIO_Pin::PIN_1);

vector<reference_wrapper<Pin>> Pin::pinVector = {PA0,PA1,PA10,PA11,PA12,
PA9,PB0,PB1,PB10,PB11,PB12,PB13,PB14,PB15,PB2,PB4,PB5,PB6,PB7,PB8,PB9,PC0,PC1,PC10,
PC11,PC12,PC13,PC14,PC15,PC2,PC3,PC4,PC5,PC6,PC7,PC8,PC9,PD0,PD1,PD10,PD11,PD12,PD13,
PD14,PD15,PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PE0,PE1,PE10,PE11,PE12,PE13,PE14,PE15,PE2,PE3,
PE4,PE5,PE6,PE7,PE8,PE9,PF0,PF1,PF10,PF11,PF12,PF13,PF14,PF15,PF2,PF3,PF4,PF5,PF6,PF7,
PF8,PF9,PG0,PG1,PG10,PG11,PG12,PG13,PG14,PG15,PG2,PG3,PG4,PG5,PG6,PG7,PG8,PG9,PH0,PH1,
PA2,PA3,PA4,PA5,PA6,PA7,PA8};

void Pin::inscribe(Pin& pin, Operation_Mode mode){
	if(pin.mode != Operation_Mode::NOT_USED){
		return;
	}
	pin.mode = mode;
}

void Pin::start(){
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	for(Pin& pin : Pin::pinVector){
		GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = pin.gpio_pin;
		switch(pin.mode){

		case Operation_Mode::NOT_USED:
			GPIO_InitStruct.Mode =  GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;

		case Operation_Mode::OUTPUT:
			GPIO_InitStruct.Mode =  GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;

		case Operation_Mode::INPUT:
			GPIO_InitStruct.Mode =  GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;

		case Operation_Mode::ANALOG:
			GPIO_InitStruct.Mode =  GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;
		case Operation_Mode::EXTERNAL_INTERRUPT:
			GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;
		case Operation_Mode::PWM_MODE:
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;

		default:
			break;
		}
	}
}







