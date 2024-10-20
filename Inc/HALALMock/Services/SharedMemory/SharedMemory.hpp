#pragma once

#include <cstdint>
#include "HALALMock/Models/PinModel/Pin.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
class SharedMemory {
   public:
    // Number of pins per GPIO port
    constexpr static uint8_t pins_per_port = 16;

    // do not use before SharedMemory::start, uninitialized!!!
    static EmulatedPin *gpio_memory;
    static EmulatedPin &get_pin(Pin pin);
    static void start();
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