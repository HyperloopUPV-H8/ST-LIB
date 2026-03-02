/*
 * SNTP.cpp
 *
 *  Created on: 21 feb. 2023
 *      Author: Ricardo
 */

#include "HALAL/Services/Communication/SNTP/SNTP.hpp"
#include "HALAL/Services/Time/RTC.hpp"

#define SUBSECONDS_PER_SECOND 32767
#define TRANSFORMATION_FACTOR (SUBSECONDS_PER_SECOND / 999999.0)

namespace {

void configure_sntp_server(const ip_addr_t& address) {
    if (sntp_enabled()) {
        sntp_stop();
    }

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setserver(0, &address);
    sntp_init();
}

} // namespace

void SNTP::sntp_update(
    uint8_t address_head,
    uint8_t address_second,
    uint8_t address_third,
    uint8_t address_last
) {
    ip_addr_t address;
    IP_ADDR4(&address, address_head, address_second, address_third, address_last);
    configure_sntp_server(address);
}

void SNTP::sntp_update(string ip) {
    IPV4 target(ip.c_str());
    configure_sntp_server(target.address);
}

void SNTP::sntp_update() { sntp_update(DEFAULT_SERVER_IP); }

extern "C" void stlib_sntp_set_rtc(
    uint16_t counter,
    uint8_t second,
    uint8_t minute,
    uint8_t hour,
    uint8_t day,
    uint8_t month,
    uint16_t year
) {
    Global_RTC::set_rtc_data(counter, second, minute, hour, day, month, year);
}

extern "C" u32_t stlib_sntp_get_rtc_seconds() {
    if (!Global_RTC::has_valid_time()) {
        return 0;
    }

    const RTCData rtc_time = Global_RTC::get_rtc_timestamp();
    time_t nowtime = 0;
    struct tm* nowtm;
    nowtm = gmtime(&nowtime);
    nowtm->tm_year = rtc_time.year - 1900;
    nowtm->tm_mon = rtc_time.month - 1;
    nowtm->tm_mday = rtc_time.day;
    nowtm->tm_hour = rtc_time.hour;
    nowtm->tm_min = rtc_time.minute;
    nowtm->tm_sec = rtc_time.second;
    u32_t sec = mktime(nowtm);
    return sec;
}

extern "C" u32_t stlib_sntp_get_rtc_microseconds() {
    if (!Global_RTC::has_valid_time()) {
        return 0;
    }

    const RTCData rtc_time = Global_RTC::get_rtc_timestamp();
    return rtc_time.counter / TRANSFORMATION_FACTOR;
}

extern "C" void stlib_sntp_set_time(uint32_t sec, uint32_t us) {
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = us;
    time_t nowtime = sec;
    struct tm* nowtm = localtime(&nowtime);
    uint32_t subsecond = (uint32_t)(TRANSFORMATION_FACTOR * tv.tv_usec);
    stlib_sntp_set_rtc(
        subsecond,
        nowtm->tm_sec,
        nowtm->tm_min,
        nowtm->tm_hour,
        nowtm->tm_mday,
        1 + nowtm->tm_mon,
        1900 + (nowtm->tm_year)
    );
}
