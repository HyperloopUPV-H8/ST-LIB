#include "HALAL/Services/Time/RTC.hpp"

RTCData Global_RTC::global_RTC{};

#ifdef HAL_RTC_MODULE_ENABLED

RTC_HandleTypeDef hrtc;

namespace {
bool rtc_started = false;
bool rtc_start_in_progress = false;
bool rtc_time_valid = false;

// Match the LSI-backed RTC setup used by STM32H7 Nucleo reference projects.
// 32 kHz / ((127 + 1) * (249 + 1)) = 1 Hz nominal calendar tick.
constexpr uint32_t rtc_async_prediv = 0x7F;
constexpr uint32_t rtc_sync_prediv = 0xF9;
} // namespace

void Global_RTC::start_rtc() {
    if (rtc_started || rtc_start_in_progress) {
        return;
    }

    rtc_start_in_progress = true;
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = rtc_async_prediv;
    hrtc.Init.SynchPrediv = rtc_sync_prediv;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;

    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        rtc_start_in_progress = false;
        PANIC("Error on RTC Init");
        return;
    }
    sTime.Hours = 0x0;
    sTime.Minutes = 0x0;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        rtc_start_in_progress = false;
        PANIC("Error while setting time at RTC start");
        return;
    }

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x1;
    sDate.Year = 23;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        rtc_start_in_progress = false;
        PANIC("Error while setting date at RTC start");
        return;
    }

    rtc_time_valid = false;
    rtc_started = true;
    rtc_start_in_progress = false;
}

bool Global_RTC::ensure_started() {
    if (!rtc_started && !rtc_start_in_progress) {
        start_rtc();
    }
    return rtc_started;
}

bool Global_RTC::is_started() { return rtc_started; }

bool Global_RTC::has_valid_time() { return rtc_time_valid; }

RTCData Global_RTC::get_rtc_timestamp() {
    RTCData ret;
    RTC_TimeTypeDef gTime;
    RTC_DateTypeDef gDate;
    HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
    ret.counter = gTime.SecondFraction - gTime.SubSeconds;
    ret.second = gTime.Seconds;
    ret.minute = gTime.Minutes;
    ret.hour = gTime.Hours;
    ret.day = gDate.Date;
    ret.month = gDate.Month;
    ret.year = 2000 + gDate.Year;
    return ret;
}

void Global_RTC::set_rtc_data(
    uint16_t counter,
    uint8_t second,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint16_t year
) {
    if (!ensure_started()) {
        return;
    }

    RTC_TimeTypeDef gTime;
    RTC_DateTypeDef gDate;
    gTime.SubSeconds = counter;
    gTime.Seconds = second;
    gTime.Minutes = minute;
    gTime.Hours = hour;
    gTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    gTime.StoreOperation = RTC_STOREOPERATION_RESET;
    gDate.WeekDay = 0;
    gDate.Date = day;
    gDate.Month = month;
    gDate.Year = year - 2000;
    bool write_ok = true;
    if (HAL_RTC_SetTime(&hrtc, &gTime, RTC_FORMAT_BIN) != HAL_OK) {
        write_ok = false;
        PANIC("Error on writing Time on the RTC");
    }
    if (HAL_RTC_SetDate(&hrtc, &gDate, RTC_FORMAT_BIN) != HAL_OK) {
        write_ok = false;
        PANIC("Error on writing Date on the RTC");
    }
    rtc_time_valid = write_ok;
    if (write_ok) {
        global_RTC = get_rtc_timestamp();
    }
}

void Global_RTC::update_rtc_data() {
    if (!ensure_started()) {
        return;
    }
    if (!rtc_time_valid) {
        global_RTC = {};
        return;
    }
    global_RTC = get_rtc_timestamp();
}

#else

void Global_RTC::start_rtc() {}

bool Global_RTC::is_started() { return false; }

bool Global_RTC::ensure_started() { return false; }

bool Global_RTC::has_valid_time() { return false; }

void Global_RTC::update_rtc_data() {}

RTCData Global_RTC::get_rtc_timestamp() { return global_RTC; }

void Global_RTC::set_rtc_data(
    uint16_t counter,
    uint8_t second,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint16_t year
) {
    global_RTC = RTCData{
        .counter = counter,
        .second = second,
        .minute = minute,
        .hour = hour,
        .day = day,
        .month = month,
        .year = year,
    };
}

#endif
