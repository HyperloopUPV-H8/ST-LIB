/*
 * Pin.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: stefan
 */
#pragma once
#include "stm32h7xx_hal.h"
#include "C++Utilities/CppUtils.hpp"

enum GPIO_Pin{
	PIN_0 = ((uint16_t)0x0001),
	PIN_1 = ((uint16_t)0x0002),
	PIN_2 = ((uint16_t)0x0004),
	PIN_3 = ((uint16_t)0x0008),
	PIN_4 = ((uint16_t)0x0010),
	PIN_5 = ((uint16_t)0x0020),
	PIN_6 = ((uint16_t)0x0040),
	PIN_7 = ((uint16_t)0x0080),
	PIN_8 = ((uint16_t)0x0100),
	PIN_9 = ((uint16_t)0x0200),
	PIN_10 = ((uint16_t)0x0400),
	PIN_11 = ((uint16_t)0x0800),
	PIN_12 = ((uint16_t)0x1000),
	PIN_13 = ((uint16_t)0x2000),
	PIN_14 = ((uint16_t)0x4000),
	PIN_15 = ((uint16_t)0x8000),
	PIN_ALL = ((uint16_t)0xFFFF)
};

enum Operation_Mode{
	NOT_USED,
	INPUT,
	OUTPUT,
	ANALOG,
	EXTERNAL_INTERRUPT,
	PWM,
	ALTERNATIVE,
};

enum PinState{
	OFF,
	ON
};

class Pin {
public:
	GPIO_TypeDef * port;
	GPIO_Pin gpio_pin;
	Operation_Mode mode = Operation_Mode::NOT_USED;
	static vector<reference_wrapper<Pin>> pinVector;

	Pin();
	Pin(GPIO_TypeDef* port, GPIO_Pin pin);
	static void inscribe(Pin& pin, Operation_Mode mode);
	static void start();

	bool operator== (const Pin &other) const {
		return (gpio_pin == other.gpio_pin && port == other.port);
	}

	bool operator< (const Pin &other) const {
		if (port == other.port)
			return gpio_pin < other.gpio_pin;
		return port < other.port;
	}
};

extern Pin PA0;
extern Pin PA1;
extern Pin PA10;
extern Pin PA11;
extern Pin PA12;
extern Pin PA13;
extern Pin PA14;
extern Pin PA15;
extern Pin PA9;
extern Pin PB0;
extern Pin PB1;
extern Pin PB10;
extern Pin PB11;
extern Pin PB12;
extern Pin PB13;
extern Pin PB14;
extern Pin PB15;
extern Pin PB2;
extern Pin PB3;
extern Pin PB4;
extern Pin PB5;
extern Pin PB6;
extern Pin PB7;
extern Pin PB8;
extern Pin PB9;
extern Pin PC0;
extern Pin PC1;
extern Pin PC10;
extern Pin PC11;
extern Pin PC12;
extern Pin PC13;
extern Pin PC14;
extern Pin PC15;
extern Pin PC2;
extern Pin PC3;
extern Pin PC4;
extern Pin PC5;
extern Pin PC6;
extern Pin PC7;
extern Pin PC8;
extern Pin PC9;
extern Pin PD0;
extern Pin PD1;
extern Pin PD10;
extern Pin PD11;
extern Pin PD12;
extern Pin PD13;
extern Pin PD14;
extern Pin PD15;
extern Pin PD2;
extern Pin PD3;
extern Pin PD4;
extern Pin PD5;
extern Pin PD6;
extern Pin PD7;
extern Pin PD8;
extern Pin PD9;
extern Pin PE0;
extern Pin PE1;
extern Pin PE10;
extern Pin PE11;
extern Pin PE12;
extern Pin PE13;
extern Pin PE14;
extern Pin PE15;
extern Pin PE2;
extern Pin PE3;
extern Pin PE4;
extern Pin PE5;
extern Pin PE6;
extern Pin PE7;
extern Pin PE8;
extern Pin PE9;
extern Pin PF0;
extern Pin PF1;
extern Pin PF10;
extern Pin PF11;
extern Pin PF12;
extern Pin PF13;
extern Pin PF14;
extern Pin PF15;
extern Pin PF2;
extern Pin PF3;
extern Pin PF4;
extern Pin PF5;
extern Pin PF6;
extern Pin PF7;
extern Pin PF8;
extern Pin PF9;
extern Pin PG0;
extern Pin PG1;
extern Pin PG10;
extern Pin PG11;
extern Pin PG12;
extern Pin PG13;
extern Pin PG14;
extern Pin PG15;
extern Pin PG2;
extern Pin PG3;
extern Pin PG4;
extern Pin PG5;
extern Pin PG6;
extern Pin PG7;
extern Pin PG8;
extern Pin PG9;
extern Pin PH0;
extern Pin PH1;
extern Pin PA2;
extern Pin PA3;
extern Pin PA4;
extern Pin PA5;
extern Pin PA6;
extern Pin PA7;
extern Pin PA8;
