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

/* NOTE: STM32h7 supports using 2 different pins for rising/falling
 *       To do this, you keep TIM_ICSELECTION_DIRECTTI when configuring both
 *       Not an option given in this interface but easily extended if necessary
 */
template <
    const ST_LIB::TimerDomain::Timer& dev,
    const ST_LIB::TimerPin pin_rising,
    const ST_LIB::TimerChannel channel_falling>
class InputCapture {
    friend struct TimerWrapper<dev>;

    TimerWrapper<dev>* timer = nullptr;
    TimerDomain::InputCaptureInfo *info = nullptr;
    bool is_on = false;

    InputCapture(TimerWrapper<dev>* tim) {
        timer = tim;

        // Setup TimerDomain
        uint8_t ch_rising = static_cast<uint8_t>(pin_rising.channel) - 1;
        uint8_t ch_falling = static_cast<uint8_t>(channel_falling) - 1;
        info =
            &TimerDomain::input_capture_info_backing[tim->instance->timer_idx][ch_rising];
        TimerDomain::input_capture_info[tim->instance->timer_idx][ch_rising] = info;
        TimerDomain::input_capture_info[tim->instance->timer_idx][ch_falling] = info;

        info->channel_rising = ch_rising;
        info->channel_falling = ch_falling;

        info->value_falling = 0;
        info->duty_cycle = 0.0f;
        info->value_rising = 0;
        info->frequency = 0;
        info->period = 0;

        timer->enable_nvic();

        TIM_IC_InitTypeDef sConfigIC = {
            .ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING,
            .ICSelection = TIM_ICSELECTION_DIRECTTI,
            .ICPrescaler = TIM_ICPSC_DIV1,
            .ICFilter = 0,
        };
        timer->template config_input_compare_channel<pin_rising.channel>(&sConfigIC);

        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
        sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
        timer->template config_input_compare_channel<channel_falling>(&sConfigIC);
    }
public:

    void turn_on(void) {
        if (is_on)
            return;

        /* STMicroelectronics' recommendation: Avoid undefined behaviour due to 
         *  first interrupt being falling instead of rising by clearing CNT and SR */        
        timer->instance->tim->CNT = 0;
        timer->instance->tim->SR = 0;

        // HAL_TIM_IC_Start_IT(instance.peripheral->handle, instance.channel_rising)
        {
            volatile HAL_TIM_ChannelStateTypeDef* ch_state =
                &timer->instance->hal_tim->ChannelState[TimerDomain::get_channel_state_idx(pin_rising.channel)];
            volatile HAL_TIM_ChannelStateTypeDef* n_ch_state =
                &timer->instance->hal_tim->ChannelNState[TimerDomain::get_channel_state_idx(pin_rising.channel)];
            if ((*ch_state != HAL_TIM_CHANNEL_STATE_READY) || (*n_ch_state != HAL_TIM_CHANNEL_STATE_READY)) {
                ErrorHandler("Channels not ready");
                return;
            }

            *ch_state = HAL_TIM_CHANNEL_STATE_BUSY;
            *n_ch_state = HAL_TIM_CHANNEL_STATE_BUSY;

            timer->template enable_capture_compare_interrupt<pin_rising.channel>();
            uint32_t enableCCx = TIM_CCER_CC1E
                            << (TimerDomain::get_channel_mul4(pin_rising.channel) & 0x1FU
                            ); /* 0x1FU = 31 bits max shift */
            SET_BIT(timer->instance->tim->CCER, enableCCx);
        }

        // HAL_TIM_IC_Start_IT(instance.peripheral->handle, instance.channel_falling)
        {
            volatile HAL_TIM_ChannelStateTypeDef* ch_state =
                &timer->instance->hal_tim->ChannelState[TimerDomain::get_channel_state_idx(channel_falling)];
            volatile HAL_TIM_ChannelStateTypeDef* n_ch_state =
                &timer->instance->hal_tim->ChannelNState[TimerDomain::get_channel_state_idx(channel_falling)];
            if ((*ch_state != HAL_TIM_CHANNEL_STATE_READY) || (*n_ch_state != HAL_TIM_CHANNEL_STATE_READY)) {
                ErrorHandler("Channels not ready");
                return;
            }

            *ch_state = HAL_TIM_CHANNEL_STATE_BUSY;
            *n_ch_state = HAL_TIM_CHANNEL_STATE_BUSY;

            timer->template enable_capture_compare_interrupt<channel_falling>();
            uint32_t enableCCx = TIM_CCER_CC1E
                            << (TimerDomain::get_channel_mul4(channel_falling) & 0x1FU
                            ); /* 0x1FU = 31 bits max shift */
            SET_BIT(timer->instance->tim->CCER, enableCCx);
        }

        if constexpr (timer->is_slave_instance) {
            uint32_t tmpsmcr = timer->instance->tim->SMCR & TIM_SMCR_SMS;
            if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr)) {
                timer->counter_enable();
            }
        } else {
            timer->counter_enable();
        }
        
        is_on = true;
    }

    void turn_off(void) {
        if(!is_on)
            return;
    
        // HAL_TIM_IC_Stop_IT(instance.peripheral->handle, instance.channel_rising)
        {
            timer->template disable_capture_compare_interrupt<pin_rising.channel>();

            CLEAR_BIT(
                timer->instance->tim->CCER,
                (uint32_t)(TIM_CCER_CC1E << (TimerDomain::get_channel_mul4(pin_rising.channel) & 0x1FU))
            );

            volatile HAL_TIM_ChannelStateTypeDef* ch_state =
                &timer->instance->hal_tim->ChannelState[TimerDomain::get_channel_state_idx(pin_rising.channel)];
            volatile HAL_TIM_ChannelStateTypeDef* n_ch_state =
                &timer->instance->hal_tim->ChannelNState[TimerDomain::get_channel_state_idx(pin_rising.channel)];
            *ch_state = HAL_TIM_CHANNEL_STATE_READY;
            *n_ch_state = HAL_TIM_CHANNEL_STATE_READY;
        }
        
        // HAL_TIM_IC_Stop_IT(instance.peripheral->handle, instance.channel_falling)
        {
            timer->template disable_capture_compare_interrupt<channel_falling>();

            CLEAR_BIT(
                timer->instance->tim->CCER,
                (uint32_t)(TIM_CCER_CC1E << (TimerDomain::get_channel_mul4(channel_falling) & 0x1FU))
            );

            volatile HAL_TIM_ChannelStateTypeDef* ch_state =
                &timer->instance->hal_tim->ChannelState[TimerDomain::get_channel_state_idx(channel_falling)];
            volatile HAL_TIM_ChannelStateTypeDef* n_ch_state =
                &timer->instance->hal_tim->ChannelNState[TimerDomain::get_channel_state_idx(channel_falling)];
            *ch_state = HAL_TIM_CHANNEL_STATE_READY;
            *n_ch_state = HAL_TIM_CHANNEL_STATE_READY;
        }
        
        if (timer->are_all_channels_free()) {
            timer->counter_disable();
        }

        is_on = false;
    }

    uint32_t get_frequency(void) {
        return info->frequency;
    }

    float get_duty_cycle(void) {
        return info->duty_cycle;
    }
};

};

#endif // HAL_TIM_MODULE_ENABLED
