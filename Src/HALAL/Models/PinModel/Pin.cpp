/*
 * Pin.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: stefa
 */

#include "Pin.hpp"

Pin::Pin(GPIO_TypeDef* Port, GPIO_Pin Pin):port(Port),pin(Pin){}

vector<reference_wrapper<Pin>> Pin::pinVector = {PA0,PA1,PA10,PA11,PA12,PA13,PA14,PA15,
PA9,PB0,PB1,PB10,PB11,PB12,PB13,PB14,PB15,PB2,PB3,PB4,PB5,PB6,PB7,PB8,PB9,PC0,PC1,PC10,
PC11,PC12,PC13,PC14,PC15,PC2,PC3,PC4,PC5,PC6,PC7,PC8,PC9,PD0,PD1,PD10,PD11,PD12,PD13,
PD14,PD15,PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PE0,PE1,PE10,PE11,PE12,PE13,PE14,PE15,PE2,PE3,
PE4,PE5,PE6,PE7,PE8,PE9,PF0,PF1,PF10,PF11,PF12,PF13,PF14,PF15,PF2,PF3,PF4,PF5,PF6,PF7,
PF8,PF9,PG0,PG1,PG10,PG11,PG12,PG13,PG14,PG15,PG2,PG3,PG4,PG5,PG6,PG7,PG8,PG9,PH0,PH1,
PA2,PA3,PA4,PA5,PA6,PA7,PA8,};

void Pin::register_pin(Pin& pin, Operation_Mode mode){
	if(pin.mode != Operation_Mode::NOT_USED){
		return;
	}
	pin.mode = mode;
}

void Pin::unregister_pin(Pin& pin){
	pin.mode = Operation_Mode::NOT_USED;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode =  GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
}

void Pin::start(){
	GPIO_InitTypeDef GPIO_InitStruct;
	for(Pin& pin : Pin::pinVector){
		GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = pin.pin;
		HAL_GPIO_WritePin(pin.port, pin.pin, GPIO_PIN_RESET);
		switch(pin.mode){

		case Operation_Mode::NOT_USED:
			GPIO_InitStruct.Mode =  GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
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

		default:
			break;
		}
	}
}




