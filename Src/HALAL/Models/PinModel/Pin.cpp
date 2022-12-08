/*
 * Pin.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: stefan
 */

#include "PinModel/Pin.hpp"

Pin::Pin(){}

Pin::Pin(GPIOPort port, GPIOPin gpio_pin) : port((GPIO_TypeDef*)port), gpio_pin(gpio_pin){}
Pin::Pin(GPIOPort port, GPIOPin gpio_pin, AlternativeFunction alternative_function) :
		port((GPIO_TypeDef*)port),
		gpio_pin(gpio_pin),
		alternative_function(alternative_function)
		{}

Pin PE2(PORT_E, PIN_2);
Pin PE3(PORT_E, PIN_3);
Pin PE4(PORT_E, PIN_4, AF5);
Pin PE5(PORT_E, PIN_5, AF5);
Pin PE6(PORT_E, PIN_6, AF5);
Pin PC13(PORT_C, PIN_13);
Pin PF0(PORT_F, PIN_0, AF14);
Pin PF1(PORT_F, PIN_1, AF14);
Pin PF2(PORT_F, PIN_2, AF14);
Pin PF3(PORT_F, PIN_3, AF14);
Pin PF4(PORT_F, PIN_4);
Pin PF5(PORT_F, PIN_5);
Pin PF6(PORT_F, PIN_6);
Pin PF7(PORT_F, PIN_7);
Pin PF8(PORT_F, PIN_8);
Pin PF9(PORT_F, PIN_9);
Pin PF10(PORT_F, PIN_10);
Pin PC0(PORT_C, PIN_0);
Pin PC1(PORT_C, PIN_1, AF12);
Pin PA0(PORT_A, PIN_0);
Pin PA1(PORT_A, PIN_1);
Pin PA2(PORT_A, PIN_2, AF12);
Pin PA3(PORT_A, PIN_3);
Pin PA4(PORT_A, PIN_4);
Pin PA5(PORT_A, PIN_5);
Pin PA6(PORT_A, PIN_6);
Pin PA7(PORT_A, PIN_7);
Pin PC4(PORT_C, PIN_4, AF12);
Pin PC5(PORT_C, PIN_5, AF12);
Pin PB0(PORT_B, PIN_0);
Pin PB1(PORT_B, PIN_1);
Pin PB2(PORT_B, PIN_2);
Pin PF11(PORT_F, PIN_11);
Pin PF12(PORT_F, PIN_12);
Pin PF13(PORT_F, PIN_13);
Pin PF14(PORT_F, PIN_14);
Pin PF15(PORT_F, PIN_15);
Pin PG0(PORT_G, PIN_0);
Pin PG1(PORT_G, PIN_1);
Pin PE7(PORT_E, PIN_7);
Pin PE8(PORT_E, PIN_8, AF2);
Pin PE9(PORT_E, PIN_9, AF2);
Pin PE10(PORT_E, PIN_10, AF2);
Pin PE11(PORT_E, PIN_11, AF2);
Pin PE12(PORT_E, PIN_12, AF2);
Pin PE13(PORT_E, PIN_13, AF2);
Pin PE14(PORT_E, PIN_14, AF2);
Pin PE15(PORT_E, PIN_15);
Pin PB10(PORT_B, PIN_10);
Pin PB11(PORT_B, PIN_11);
Pin PB12(PORT_B, PIN_12);
Pin PB13(PORT_B, PIN_13, AF12);
Pin PB14(PORT_B, PIN_14, AF3);
Pin PB15(PORT_B, PIN_15, AF3);
Pin PD8(PORT_D, PIN_8);
Pin PD9(PORT_D, PIN_9);
Pin PD10(PORT_D, PIN_10);
Pin PD11(PORT_D, PIN_11);
Pin PD12(PORT_D, PIN_12, AF3);
Pin PD13(PORT_D, PIN_13, AF3);
Pin PD14(PORT_D, PIN_14, AF3);
Pin PD15(PORT_D, PIN_15, AF3);
Pin PG2(PORT_G, PIN_2);
Pin PG3(PORT_G, PIN_3);
Pin PG4(PORT_G, PIN_4);
Pin PG5(PORT_G, PIN_5);
Pin PG6(PORT_G, PIN_6);
Pin PG7(PORT_G, PIN_7);
Pin PG8(PORT_G, PIN_8);
Pin PC6(PORT_C, PIN_6, AF4);
Pin PC7(PORT_C, PIN_7, AF4);
Pin PC8(PORT_C, PIN_8, AF3);
Pin PC9(PORT_C, PIN_9, AF3);
Pin PA8(PORT_A, PIN_8);
Pin PA9(PORT_A, PIN_9, AF8);
Pin PA10(PORT_A, PIN_10, AF8);
Pin PA11(PORT_A, PIN_11, AF10);
Pin PA12(PORT_A, PIN_12, AF10);
Pin PC10(PORT_C, PIN_10);
Pin PC11(PORT_C, PIN_11);
Pin PC12(PORT_C, PIN_12);
Pin PD0(PORT_D, PIN_0);
Pin PD1(PORT_D, PIN_1);
Pin PD2(PORT_D, PIN_2);
Pin PD3(PORT_D, PIN_3);
Pin PD4(PORT_D, PIN_4);
Pin PD5(PORT_D, PIN_5, AF8);
Pin PD6(PORT_D, PIN_6, AF8);
Pin PD7(PORT_D, PIN_7);
Pin PG9(PORT_G, PIN_9, AF3);
Pin PG10(PORT_G, PIN_10, AF3);
Pin PG11(PORT_G, PIN_11, AF12);
Pin PG12(PORT_G, PIN_12);
Pin PG13(PORT_G, PIN_13, AF12);
Pin PG14(PORT_G, PIN_14);
Pin PG15(PORT_G, PIN_15);
Pin PB5(PORT_B, PIN_5, AF3);
Pin PB6(PORT_B, PIN_6, AF2);
Pin PB7(PORT_B, PIN_7, AF2);
Pin PB8(PORT_B, PIN_8, AF2);
Pin PB9(PORT_B, PIN_9, AF2);
Pin PE0(PORT_E, PIN_0);
Pin PE1(PORT_E, PIN_1);


vector<reference_wrapper<Pin>> Pin::pinVector = {PA0,PA1,PA10,PA11,PA12,
PA9,PB0,PB1,PB10,PB11,PB12,PB13,PB14,PB15,PB2,PB4,PB5,PB6,PB7,PB8,PB9,PC0,PC1,PC10,
PC11,PC12,PC13,PC14,PC15,PC2,PC3,PC4,PC5,PC6,PC7,PC8,PC9,PD0,PD1,PD10,PD11,PD12,PD13,
PD14,PD15,PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PE0,PE1,PE10,PE11,PE12,PE13,PE14,PE15,PE2,PE3,
PE4,PE5,PE6,PE7,PE8,PE9,PF0,PF1,PF10,PF11,PF12,PF13,PF14,PF15,PF2,PF3,PF4,PF5,PF6,PF7,
PF8,PF9,PG0,PG1,PG10,PG11,PG12,PG13,PG14,PG15,PG2,PG3,PG4,PG5,PG6,PG7,PG8,PG9,PH0,PH1,
PA2,PA3,PA4,PA5,PA6,PA7,PA8};

void Pin::inscribe(Pin& pin, OperationMode mode){
	if(pin.mode != OperationMode::NOT_USED){
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

		case OperationMode::NOT_USED:
			GPIO_InitStruct.Mode =  GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;

		case OperationMode::OUTPUT:
			GPIO_InitStruct.Mode =  GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;

		case OperationMode::INPUT:
			GPIO_InitStruct.Mode =  GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;

		case OperationMode::ANALOG:
			GPIO_InitStruct.Mode =  GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;
		case OperationMode::EXTERNAL_INTERRUPT:
			GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;
		case OperationMode::ALTERNATIVE:
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = pin.alternative_function;
			HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
			break;

		default:
			break;
		}
	}
}







