#include "HALAL/Services/Diagnostics/Diagnostics.hpp"

#include "HALAL/Services/Time/Scheduler.hpp"

#if defined(HAL_RTC_MODULE_ENABLED) && !defined(SIM_ON)
#include "HALAL/Services/Time/RTC.hpp"
#endif

namespace Diagnostics {

Timestamp DiagnosticTimestampProvider::capture() {
    Timestamp timestamp{};

#if defined(HAL_RTC_MODULE_ENABLED) && !defined(SIM_ON)
    if (Global_RTC::ensure_started() && Global_RTC::has_valid_time()) {
        Global_RTC::update_rtc_data();
        const RTCData& rtc = Global_RTC::global_RTC;
        timestamp.has_rtc = true;
        timestamp.counter = rtc.counter;
        timestamp.second = rtc.second;
        timestamp.minute = rtc.minute;
        timestamp.hour = rtc.hour;
        timestamp.day = rtc.day;
        timestamp.month = rtc.month;
        timestamp.year = rtc.year;
        return timestamp;
    }
#endif

#ifdef HAL_TIM_MODULE_ENABLED
    timestamp.uptime_us = Scheduler::get_global_tick();
#endif
    return timestamp;
}

} // namespace Diagnostics
