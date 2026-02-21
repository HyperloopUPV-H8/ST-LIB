/*
 * InputCapture.hpp
 *
 *  Created on: 17 feb. 2026
 *      Author: victor
 */
#pragma once

#include "HALAL/Models/TimerDomain/TimerDomain.hpp"
#ifdef HAL_TIM_MODULE_ENABLED
#include "HALAL/Models/GPIO.hpp"

namespace ST_LIB {

template <const TimerDomain::Timer& dev> struct TimerWrapper;

template <
    const ST_LIB::TimerDomain::Timer& dev,
    const ST_LIB::TimerPin pin_rising,
    const ST_LIB::TimerPin pin_falling>
class InputCapture {
    TimerWrapper<dev>* timer;
    uint32_t frequency;
    uint8_t duty_cycle;
    bool is_on;

public:
    InputCapture(TimerWrapper<dev>* tim) : timer(tim) {
        TIM_IC_InitTypeDef sConfigIC = {
            .ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING,
            .ICPrescaler = TIM_ICPSC_DIV1,
            .ICFilter = 0,
            .ICSelection = TIM_ICSELECTION_DIRECTTI,
        };
        timer->template config_input_compare_channel<pin_rising.channel>(&sConfigIC);

        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
        sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
        timer->template config_input_compare_channel<pin_falling.channel>(&sConfigIC);
    }

    static void turn_on(void) {
        if (this->is_on)
            return;
        
        // HAL_TIM_IC_Start_IT(instance.peripheral->handle, instance.channel_rising)
        {
            volatile HAL_TIM_ChannelStateTypeDef* ch_state =
                &timer->instance->hal_tim->ChannelState[TimerDomain::get_channel_state_idx(pin_rising.channel)];
            volatile HAL_TIM_ChannelStateTypeDef* n_ch_state =
                &timer->instance->hal_tim->ChannelNState[TimerDomain::get_channel_state_idx(pin_rising.channel)];
            if ((*ch_state != HAL_TIM_CHANNEL_STATE_READY) || (*n_ch_state != HAL_TIM_CHANNEL_STATE_READY)) {
                ErrorHandler("Channels not ready");
            }

            *ch_state = HAL_TIM_CHANNEL_STATE_BUSY;
            *n_ch_state = HAL_TIM_CHANNEL_STATE_BUSY;

            timer->template enable_capture_compare_interrupt<pin_rising.channel>();
            uint32_t enableCCx = TIM_CCER_CC1E
                            << (TimerDomain::get_channel_mul4(pin_rising.channel) & 0x1FU
                            ); /* 0x1FU = 31 bits max shift */
            SET_BIT(timer->instance->tim->CCER, enableCCx);
        }

        // HAL_TIM_IC_Start(instance.peripheral->handle, instance.channel_falling)
        {
            volatile HAL_TIM_ChannelStateTypeDef* ch_state =
                &timer->instance->hal_tim->ChannelState[TimerDomain::get_channel_state_idx(pin_falling.channel)];
            volatile HAL_TIM_ChannelStateTypeDef* n_ch_state =
                &timer->instance->hal_tim->ChannelNState[TimerDomain::get_channel_state_idx(pin_falling.channel)];
            if ((*ch_state != HAL_TIM_CHANNEL_STATE_READY) || (*n_ch_state != HAL_TIM_CHANNEL_STATE_READY)) {
                ErrorHandler("Channels not ready");
            }

            *ch_state = HAL_TIM_CHANNEL_STATE_BUSY;
            *n_ch_state = HAL_TIM_CHANNEL_STATE_BUSY;

            uint32_t enableCCx = TIM_CCER_CC1E
                            << (TimerDomain::get_channel_mul4(pin_falling.channel) & 0x1FU
                            ); /* 0x1FU = 31 bits max shift */
            SET_BIT(timer->instance->tim->CCER, enableCCx);
        }

        if constexpr (timer->is_slave_instance) {
            uint32_t tmpsmcr = timer->instance->tim->SMCR & SMCR_SMS;
            if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr)) {
                timer->counter_enable();
            }
        } else {
            timer->counter_enable();
        }
        
        this->is_on = true;
    }

    static void turn_off(void) {
        if(!this->is_on)
            return;
    
        // HAL_TIM_IC_Stop_IT(instance.peripheral->handle, instance.channel_rising)
        {
            timer->template disable_capture_compare_interrupt<pin_rising.channel>();

            CLEAR_BIT(
                timer->instance->tim->CCER,
                (uint32_t)(TIM_CCER_CC1E << (TimerDomain::get_channel_mul4(pin_rising.channel) & 0x1FU))
            );

            volatile HAL_TIM_ChannelStateTypeDef* ch_state =
                &timer->instance->hal_tim->ChannelState[TimerDomain::get_channel_state_idx(pin_falling.channel)];
            volatile HAL_TIM_ChannelStateTypeDef* n_ch_state =
                &timer->instance->hal_tim->ChannelNState[TimerDomain::get_channel_state_idx(pin_falling.channel)];
            *ch_state = HAL_TIM_CHANNEL_STATE_READY;
            *ch_n_state = HAL_TIM_CHANNEL_STATE_READY;
        }
        
        // HAL_TIM_IC_Stop(instance.peripheral->handle, instance.channel_falling)
        {
            CLEAR_BIT(
                timer->instance->tim->CCER,
                (uint32_t)(TIM_CCER_CC1E << (TimerDomain::get_channel_mul4(pin_rising.channel) & 0x1FU))
            );

            volatile HAL_TIM_ChannelStateTypeDef* ch_state =
                &timer->instance->hal_tim->ChannelState[TimerDomain::get_channel_state_idx(pin_falling.channel)];
            volatile HAL_TIM_ChannelStateTypeDef* n_ch_state =
                &timer->instance->hal_tim->ChannelNState[TimerDomain::get_channel_state_idx(pin_falling.channel)];
            *ch_state = HAL_TIM_CHANNEL_STATE_READY;
            *ch_n_state = HAL_TIM_CHANNEL_STATE_READY;
        }
        
        if (timer->are_all_channels_free()) {
            timer->counter_disable();
        }

#error TODO
        if (HAL_TIM_IC_Stop(instance.peripheral->handle, instance.channel_falling) != HAL_OK) {
            ErrorHandler(
                "Unable to stop the %s Input Capture measurement",
                instance.peripheral->name.c_str()
            );
        }


        this->is_on = false;
    }

    static uint32_t read_frequency(void) {
        return this->frequency;
    }

    static uint8_t read_duty_cycle(void) {
        return this->duty_cycle;
    }
};

};

#endif // HAL_TIM_MODULE_ENABLED
