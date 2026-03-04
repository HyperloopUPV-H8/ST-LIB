#pragma once

#include "HALAL/Models/TimerDomain/TimerDomain.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

#define NANO_SECOND 1000000000.0
#define CLOCK_MAX_VALUE 4294967295 // here goes the tim23 counter period

namespace ST_LIB {

template <const TimerDomain::Timer& dev> struct TimerWrapper;

template <const TimerDomain::Timer& dev> class Encoder {
    static_assert(
        dev.e.pin_count == 2,
        "Encoder must have exactly 2 encoder pins, as it uses the whole timer"
    );
    static_assert(dev.e.pins[0].af == TimerAF::Encoder, "Pin 0 must be declared as encoder");
    static_assert(dev.e.pins[1].af == TimerAF::Encoder, "Pin 1 must be declared as encoder");
    static_assert(
        dev.e.pins[0].channel != dev.e.pins[1].channel,
        "Pins must be of different channels"
    );

    inline static TimerWrapper<dev>* timer;
    inline static bool is_on = false;

    static TimerWrapper<dev>* require_timer_wrapper(const char* error_message) {
        if (timer == nullptr) {
            ErrorHandler(error_message);
            return nullptr;
        }
        return timer;
    }

    static TimerDomain::Instance* require_counter_instance(const char* error_message) {
        TimerWrapper<dev>* timer_wrapper = require_timer_wrapper(error_message);
        if (timer_wrapper == nullptr) {
            return nullptr;
        }
        if (timer_wrapper->instance == nullptr || timer_wrapper->instance->tim == nullptr) {
            ErrorHandler(error_message);
            return nullptr;
        }
        return timer_wrapper->instance;
    }

    static TimerDomain::Instance* require_hal_instance(const char* error_message) {
        TimerDomain::Instance* instance = require_counter_instance(error_message);
        if (instance == nullptr) {
            return nullptr;
        }
        if (instance->hal_tim == nullptr) {
            ErrorHandler(error_message);
            return nullptr;
        }
        return instance;
    }

public:
    Encoder(TimerWrapper<dev>* tim) {
        if (timer == nullptr) {
            init(tim);
        }
    }

    static void init(TimerWrapper<dev>* tim) {
        if (tim == nullptr || tim->instance == nullptr || tim->instance->tim == nullptr ||
            tim->instance->hal_tim == nullptr) {
            ErrorHandler("Unable to init encoder, timer is null");
            return;
        }

        TIM_Encoder_InitTypeDef sConfig = {0};
        TIM_MasterConfigTypeDef sMasterConfig = {0};

        sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
        sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
        sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
        sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
        sConfig.IC1Filter = 0;
        sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
        sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
        sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
        sConfig.IC2Filter = 0;

        if (HAL_TIM_Encoder_Init(tim->instance->hal_tim, &sConfig) != HAL_OK) {
            ErrorHandler("Unable to init encoder");
            return;
        }

        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(tim->instance->hal_tim, &sMasterConfig) !=
            HAL_OK) {
            ErrorHandler("Unable to config master synchronization in encoder");
            return;
        }

        tim->instance->tim->PSC = 5;
        tim->instance->tim->ARR = 55000;
        timer = tim;
    }

    static void turn_on() {
        if (is_on)
            return;
        TimerDomain::Instance* instance =
            require_hal_instance("Unable to start encoder, timer is not initialized");
        if (instance == nullptr) {
            return;
        }

        if (HAL_TIM_Encoder_GetState(instance->hal_tim) == HAL_TIM_STATE_RESET) {
            ErrorHandler("Unable to get state from encoder");
            return;
        }
        if (HAL_TIM_Encoder_Start(instance->hal_tim, TIM_CHANNEL_ALL) != HAL_OK) {
            ErrorHandler("Unable to start encoder");
            return;
        }
        is_on = true;
        reset();
    }

    static void turn_off() {
        if (!is_on)
            return;
        TimerDomain::Instance* instance =
            require_hal_instance("Unable to stop encoder, timer is not initialized");
        if (instance == nullptr) {
            return;
        }
        if (HAL_TIM_Encoder_Stop(instance->hal_tim, TIM_CHANNEL_ALL) != HAL_OK) {
            ErrorHandler("Unable to stop encoder");
            return;
        }
        is_on = false;
    }

    static inline void reset() {
        TimerDomain::Instance* instance =
            require_counter_instance("Unable to reset encoder, timer is not initialized");
        if (instance == nullptr) {
            return;
        }
        instance->tim->CNT = get_initial_counter_value();
    }

    static inline uint32_t get_counter() {
        TimerDomain::Instance* instance =
            require_counter_instance("Unable to read encoder counter, timer is not initialized");
        if (instance == nullptr) {
            return 0;
        }
        return instance->tim->CNT;
    }

    static inline bool get_direction() {
        TimerDomain::Instance* instance =
            require_counter_instance("Unable to read encoder direction, timer is not initialized");
        if (instance == nullptr) {
            return false;
        }
        return ((instance->tim->CR1 & 0b10000) >> 4);
    }

    static inline uint32_t get_initial_counter_value() {
        TimerDomain::Instance* instance = require_counter_instance(
            "Unable to read encoder initial counter, timer is not initialized"
        );
        if (instance == nullptr) {
            return 0;
        }
        return instance->tim->ARR / 2;
    }

    static int64_t get_delta_clock(uint64_t clock_time, uint64_t last_clock_time) {
        TimerWrapper<dev>* timer_wrapper =
            require_timer_wrapper("Unable to compute encoder delta clock, timer is not initialized");
        if (timer_wrapper == nullptr) {
            return 0;
        }
        int64_t delta_clock = clock_time - last_clock_time;
        if (clock_time < last_clock_time) { // overflow handle
            delta_clock = clock_time +
                          CLOCK_MAX_VALUE * NANO_SECOND / timer_wrapper->get_clock_frequency() -
                          last_clock_time;
        }
        return delta_clock;
    }
};

} // namespace ST_LIB
#endif
