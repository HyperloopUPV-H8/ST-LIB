#include "HALAL/Models/TimerDomain/TimerDomain.hpp"

using namespace ST_LIB;

#define CaptureCompareInterruptMask \
    (TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF)

#define CaptureCompareOvercaptureMask \
    (TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF)

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim12;
TIM_HandleTypeDef htim13;
TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;
TIM_HandleTypeDef htim23;
TIM_HandleTypeDef htim24;

void (*TimerDomain::callbacks[TimerDomain::max_instances])(void*) = {nullptr};
void* TimerDomain::callback_data[TimerDomain::max_instances] = {nullptr};
TimerDomain::InputCaptureInfo* TimerDomain::input_capture_info[max_instances][input_capture_channels];
TimerDomain::InputCaptureInfo TimerDomain::input_capture_info_backing[max_instances][input_capture_channels];

static void TIM_IC_CaptureCallback(const uint32_t timer_idx, uint32_t cc_channel)
{
    TIM_HandleTypeDef* htim = TimerDomain::hal_handles[timer_idx];
    htim->Instance->CNT = 0;

    uint32_t channel = __builtin_ffs(cc_channel) - 2;
    TimerDomain::InputCaptureInfo* info = TimerDomain::input_capture_info[timer_idx][channel];
    if(info->channel_rising == channel) {
        // NOTE: CCR1 - CCR4 are contiguous
        // NOTE: CCxIF flag is cleared by software by reading the captured data in CCRx
        info->value_rising = (float)(*(((uint32_t*)&htim->Instance->CCR1) + channel));
        
        if((htim->Instance->SR & CaptureCompareOvercaptureMask) != 0) [[unlikely]] {
            CLEAR_BIT(htim->Instance->SR, CaptureCompareOvercaptureMask);
        }

        uint32_t ref_clock = TimerDomain::get_timer_frequency(htim->Instance) / (htim->Instance->PSC + 1);
        info->frequency = (uint32_t)((ref_clock / info->value_rising) + 0.5f);
    } else if(info->channel_falling == channel) {
        uint32_t falling_value = *(((uint32_t*)&htim->Instance->CCR1) + channel);

        info->duty_cycle = ((float)falling_value * 100.0f) / info->value_rising;
    } else [[unlikely]] {
        ErrorHandler("TimerDomain::input_capture_info was modified");
    }
}

static void TIM_InterruptCallback(const uint32_t timer_idx)
{
    TIM_TypeDef* tim = TimerDomain::cmsis_timers[timer_idx];
    if(tim->SR & TIM_SR_UIF) {
        CLEAR_BIT(tim->SR, TIM_SR_UIF);
        TimerDomain::callbacks[timer_idx](TimerDomain::callback_data[timer_idx]);
    }
    
    uint32_t cc_channel = tim->SR & CaptureCompareInterruptMask;
    if(cc_channel != 0) {
        TIM_IC_CaptureCallback(timer_idx, cc_channel);
    }
}

extern "C" void TIM1_UP_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[1]);
}

extern "C" void TIM2_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[2]);
}

extern "C" void TIM3_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[3]);
}

extern "C" void TIM4_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[4]);
}

extern "C" void TIM5_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[5]);
}

extern "C" void TIM6_DAC_IRQHandler(void) {
    // NOTE: Basic timers have no capture compare channels
    CLEAR_BIT(TimerDomain::cmsis_timers[timer_idxmap[6]]->SR, TIM_SR_UIF);
    TimerDomain::callbacks[timer_idxmap[6]](TimerDomain::callback_data[timer_idxmap[6]]);
}

extern "C" void TIM7_IRQHandler(void) {
    // NOTE: Basic timers have no capture compare channels
    CLEAR_BIT(TimerDomain::cmsis_timers[timer_idxmap[7]]->SR, TIM_SR_UIF);
    TimerDomain::callbacks[timer_idxmap[7]](TimerDomain::callback_data[timer_idxmap[7]]);
}

extern "C" void TIM8_BRK_TIM12_IRQHandler(void) {
    constexpr uint32_t tim8_idx = timer_idxmap[8];
    constexpr uint32_t tim12_idx = timer_idxmap[12];

    TIM_TypeDef* tim8 = TimerDomain::cmsis_timers[tim8_idx];
    TIM_TypeDef* tim12 = TimerDomain::cmsis_timers[tim12_idx];
    
    uint32_t tim12_cc_channel = tim12->SR & CaptureCompareInterruptMask;
    if ((tim12->SR & TIM_SR_UIF) != 0) {
        CLEAR_BIT(tim12->SR, TIM_SR_UIF);
        TimerDomain::callbacks[tim12_idx](TimerDomain::callback_data[tim12_idx]);
    }
    if (tim12_cc_channel != 0) {
        TIM_IC_CaptureCallback(tim12_idx, tim12_cc_channel);
    }

    if ((tim8->SR & TIM_SR_BIF) != 0) {
        CLEAR_BIT(tim8->SR, TIM_SR_BIF);
        /* this could probably have some other callback */
        TimerDomain::callbacks[tim8_idx](TimerDomain::callback_data[tim8_idx]);
    }
}

extern "C" void TIM8_UP_TIM13_IRQHandler(void) {
    constexpr uint32_t tim8_idx = timer_idxmap[8];
    constexpr uint32_t tim13_idx = timer_idxmap[13];

    TIM_TypeDef* tim8 = TimerDomain::cmsis_timers[tim8_idx];
    TIM_TypeDef* tim13 = TimerDomain::cmsis_timers[tim13_idx];
    
    uint32_t tim8_cc_channel = tim8->SR & CaptureCompareInterruptMask;
    uint32_t tim13_cc_channel = tim13->SR & CaptureCompareInterruptMask;
    if ((tim13->SR & TIM_SR_UIF) != 0) {
        CLEAR_BIT(tim13->SR, TIM_SR_UIF);
        TimerDomain::callbacks[tim13_idx](TimerDomain::callback_data[tim13_idx]);
    }
    if (tim13_cc_channel != 0) {
        TIM_IC_CaptureCallback(tim13_idx, tim13_cc_channel);
    }

    if ((tim8->SR & TIM_SR_UIF) != 0) {
        CLEAR_BIT(tim8->SR, TIM_SR_UIF);
        TimerDomain::callbacks[tim8_idx](TimerDomain::callback_data[tim8_idx]);
    }
    if (tim8_cc_channel != 0) {
        TIM_IC_CaptureCallback(tim8_idx, tim8_cc_channel);
    }
}

extern "C" void TIM8_TRG_COM_TIM14_IRQHandler(void) {
    constexpr uint32_t tim8_idx = timer_idxmap[8];
    constexpr uint32_t tim14_idx = timer_idxmap[14];

    TIM_TypeDef* tim8 = TimerDomain::cmsis_timers[tim8_idx];
    TIM_TypeDef* tim14 = TimerDomain::cmsis_timers[tim14_idx];

    uint32_t tim14_cc_channel = tim14->SR & CaptureCompareInterruptMask;
    if ((tim14->SR & TIM_SR_UIF) != 0) {
        CLEAR_BIT(tim14->SR, TIM_SR_UIF);
        TimerDomain::callbacks[tim14_idx](TimerDomain::callback_data[tim14_idx]);
    }
    if (tim14_cc_channel != 0) {
        TIM_IC_CaptureCallback(tim14_idx, tim14_cc_channel);
    }

    constexpr uint32_t com_trg_flags = TIM_SR_TIF | TIM_SR_COMIF;
    if ((tim8->SR & com_trg_flags) != 0) {
        CLEAR_BIT(tim8->SR, com_trg_flags);
        /* this could probably have some other callback */
        TimerDomain::callbacks[tim8_idx](TimerDomain::callback_data[tim8_idx]);
    }
}

extern "C" void TIM15_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[15]);
}

extern "C" void TIM16_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[16]);
}

extern "C" void TIM17_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[17]);
}

extern "C" void TIM23_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[23]);
}

extern "C" void TIM24_IRQHandler(void) {
    TIM_InterruptCallback(timer_idxmap[24]);
}
