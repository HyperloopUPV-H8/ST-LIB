/*
 * Pin.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: stefan
 */
#pragma once
#include "C++Utilities/CppUtils.hpp"

#include "stm32h7xx_hal.h"

#define PERIPHERAL_BASE 0x40000000UL
#define DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 PERIPHERAL_BASE+0x18020000UL

enum GPIOPin : uint16_t{
	PIN_0 = 0x0001,
	PIN_1 = 0x0002,
	PIN_2 = 0x0004,
	PIN_3 = 0x0008,
	PIN_4 = 0x0010,
	PIN_5 = 0x0020,
	PIN_6 = 0x0040,
	PIN_7 = 0x0080,
	PIN_8 = 0x0100,
	PIN_9 = 0x0200,
	PIN_10 = 0x0400,
	PIN_11 = 0x0800,
	PIN_12 = 0x1000,
	PIN_13 = 0x2000,
	PIN_14 = 0x4000,
	PIN_15 = 0x8000,
	PIN_ALL = 0xFFFF
};

enum GPIOPort : uint32_t {
	PORT_A = DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 + 0x0000UL,
	PORT_B = DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 + 0x0400UL,
	PORT_C = DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 + 0x0800UL,
	PORT_D = DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 + 0x0C00UL,
	PORT_E = DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 + 0x1000UL,
	PORT_F = DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 + 0x1400UL,
	PORT_G = DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 + 0x1800UL,
	PORT_H = DOMAIN3_ADVANCED_HIGH_PERFORMANCE_BUS1 + 0x1C00UL,
};

enum AlternativeFunction: uint8_t {
	AF0 = 0x00,
	AF1 = 0x01,
	AF2 = 0x02,
	AF3 = 0x03,
	AF4 = 0x04,
	AF5 = 0x05,
	AF6 = 0x06,
	AF7 = 0x07,
	AF8 = 0x08,
	AF9 = 0x09,
	AF10 = 0x0A,
	AF11 = 0x0B,
	AF12 = 0x0C,
	AF13 = 0x0D,
	AF14 = 0x0E,
	AF15 = 0x0F,
};

enum OperationMode{
	NOT_USED,
	INPUT,
	OUTPUT,
	ANALOG,
	EXTERNAL_INTERRUPT_RISING,
	EXTERNAL_INTERRUPT_FALLING,
	TIMER_ALTERNATE_FUNCTION,
	ALTERNATIVE,
};

enum PinState{
	OFF,
	ON
};

enum PinType {
	NOT_USED,
	DigitalOutput,
	DigitalInput,
	PWM,
	ADC
	// TODO: Add more types
};

struct PinModel {
	PinType type=PinType::NOT_USED; // Always check type before using the union

	union  {
		struct DigitalOutput {
			// TODO FW-52
		}DigitalOutput;
		struct  {
			// TODO FW-53
		}DigitalInput;
		struct  {
			float duty_cycle;
			uint32_t frequency;
			bool is_on;
		}PWM;
		struct {
			// TODO FW-54
		}ADC;
		// TODO Add more types
	}PinData;
};

unordered_map<Pin, size_t> pin_offsets = {
	{PA0, 0}, {PA1, 1}, {PA2, 2}, {PA3, 3}, {PA4, 4},
	{PA5, 5}, {PA6, 6}, {PA7, 7}, {PA8, 8}, {PA9, 9},
	{PA10, 10}, {PA11, 11}, {PA12, 12}, {PA13, 13}, 
	{PA14, 14}, {PA15, 15}, {PB0, 16}, {PB1, 17},
	{PB2, 18}, {PB3, 19}, {PB4, 20}, {PB5, 21},
	{PB6, 22}, {PB7, 23}, {PB8, 24}, {PB9, 25},
	{PB10, 26}, {PB11, 27}, {PB12, 28}, {PB13, 29},
	{PB14, 30}, {PB15, 31}, {PC0, 32}, {PC1, 33},
	{PC2, 34}, {PC3, 35}, {PC4, 36}, {PC5, 37},
	{PC6, 38}, {PC7, 39}, {PC8, 40}, {PC9, 41},
	{PC10, 42}, {PC11, 43}, {PC12, 44}, {PC13, 45},
	{PC14, 46}, {PC15, 47}, {PD0, 48}, {PD1, 49},
	{PD2, 50}, {PD3, 51}, {PD4, 52}, {PD5, 53},
	{PD6, 54}, {PD7, 55}, {PD8, 56}, {PD9, 57},
	{PD10, 58}, {PD11, 59}, {PD12, 60}, {PD13, 61},
	{PD14, 62}, {PD15, 63}, {PE0, 64}, {PE1, 65},
	{PE2, 66}, {PE3, 67}, {PE4, 68}, {PE5, 69},
	{PE6, 70}, {PE7, 71}, {PE8, 72}, {PE9, 73},
	{PE10, 74}, {PE11, 75}, {PE12, 76}, {PE13, 77},
	{PE14, 78}, {PE15, 79}, {PF0, 80}, {PF1, 81},
	{PF2, 82}, {PF3, 83}, {PF4, 84}, {PF5, 85},
	{PF6, 86}, {PF7, 87}, {PF8, 88}, {PF9, 89},
	{PF10, 90}, {PF11, 91}, {PF12, 92}, {PF13, 93},
	{PF14, 94}, {PF15, 95}, {PG0, 96}, {PG1, 97},
	{PG2, 98}, {PG3, 99}, {PG4, 100}, {PG5, 101},
	{PG6, 102}, {PG7, 103}, {PG8, 104}, {PG9, 105},
	{PG10, 106}, {PG11, 107}, {PG12, 108}, {PG13, 109},
	{PG14, 110}, {PG15, 111}, {PH0, 112}, {PH1, 113}
};

class Pin {
public:
	GPIO_TypeDef * port;
	GPIOPin gpio_pin;
	AlternativeFunction alternative_function;
	OperationMode mode = OperationMode::NOT_USED;
	static const vector<reference_wrapper<Pin>> pinVector;
	static const map<GPIO_TypeDef*,const string> port_to_string;
	static const map<GPIOPin,const string> gpio_pin_to_string;
	Pin();
	Pin(GPIOPort port, GPIOPin pin);
	Pin(GPIOPort port, GPIOPin pin, AlternativeFunction alternative_function);
	const string to_string() const;
	static void inscribe(Pin& pin, OperationMode mode);
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

namespace std {
	template <>
	struct hash<Pin> {
		std::size_t operator()(const Pin& k) const {
		    using std::size_t;
		    using std::hash;
		    using std::string;

		    return ((hash<uint16_t>()(k.gpio_pin) ^ (hash<uint32_t>()((uint32_t)(k.port)) << 1)) >> 1);
		}
	  };
}

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
