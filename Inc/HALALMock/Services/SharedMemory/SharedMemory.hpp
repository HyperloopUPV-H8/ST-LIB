#pragma once

#include <cstdint>

class SharedMemory {
   public:
    // Number of pins per GPIO port
    constexpr static uint8_t pins_per_port = 16;
    constexpr static uint8_t max_pin_size_bytes{16};

    // do not use before SharedMemory::start, uninitialized!!!
    static void *gpio_memory;

    static void start();
};