#include "HALAL/Services/Watchdog/Watchdog.hpp"
#include "HALAL/HardFault/HardfaultTrace.h"

IWDG_HandleTypeDef watchdog_handle;
std::chrono::microseconds Watchdog::watchdog_time =
    std::chrono::microseconds(1000000); // 1 second by default
bool reset_by_iwdg{};

#ifdef NUCLEO
#define WDG_REPS 200000
#endif
#ifdef BOARD
#define WDG_REPS 800000
#endif

__attribute__((optimize("O0"))) static void wdg_delay() {
    for (uint32_t i = 0; i < WDG_REPS; i++) {
        __NOP();
    }
}

static void wdg_enable_gpio_clock(GPIO_TypeDef* port) {
    if (port == GPIOA)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);
    else if (port == GPIOB)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
    else if (port == GPIOC)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
    else if (port == GPIOD)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);
    else if (port == GPIOE)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE);
    else if (port == GPIOF)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF);
    else if (port == GPIOG)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG);
    else if (port == GPIOJ)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOJ);
    else if (port == GPIOK)
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOK);
}

static void wdg_init_gpio_output(GPIO_TypeDef* port, uint16_t pin) {
    LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(port, pin, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinPull(port, pin, LL_GPIO_PULL_NO);
}

static void wdg_led_init() {
    for (int i = 0; i < hard_fault_leds_count; i++) {
        wdg_enable_gpio_clock(ports_hard_fault[i]);
        wdg_init_gpio_output(ports_hard_fault[i], pins_hard_fault[i]);
    }
}

__attribute__((optimize("O0"))) void Watchdog::check_reset_flag() {
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST)) {
        reset_by_iwdg = true;
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();

    if (reset_by_iwdg) {
        if (hard_fault_leds_count == 0U) {
            while (1)
                ;
        }
        wdg_led_init();
        while (1) {
            for (int i = 0; i < hard_fault_leds_count; i++) {
                LL_GPIO_SetOutputPin(ports_hard_fault[i], pins_hard_fault[i]);
                wdg_delay();
                LL_GPIO_ResetOutputPin(ports_hard_fault[i], pins_hard_fault[i]);
            }
        }
    }
}
