/*
 * InfoWarning.cpp
 *
 *  Created on: Jun 12, 2024
 *      Author: gonzalo
 */

#include "HALAL/Services/InfoWarning/InfoWarning.hpp"
#include "HALAL/Services/Communication/UART/UART.hpp"
#include "HALAL/Services/Time/RTC.hpp"
#include "HALAL/Services/Time/Scheduler.hpp"
#ifdef STLIB_ETH
#include "HALAL/Models/Packets/Order.hpp"
#endif

namespace {

#ifdef STLIB_ETH
constexpr uint16_t INFO_WARNING_TCP_ORDER_ID = 2555;
constexpr uint8_t INFO_WARNING_BOUNDARY_TYPE_ID = 5;
#endif

bool warning_sent_via_tcp = false;
bool warning_sent_via_uart = false;
bool tcp_delivery_required = false;
bool uart_delivery_required = false;

#ifdef STLIB_ETH
uint8_t warning_padding = 0;
uint8_t warning_boundary_type = INFO_WARNING_BOUNDARY_TYPE_ID;
string warning_name = "info_warning";
string warning_message = "Warning-No-Description-Found";
uint16_t warning_counter = 0;
uint8_t warning_second = 0;
uint8_t warning_minute = 0;
uint8_t warning_hour = 0;
uint8_t warning_day = 0;
uint8_t warning_month = 0;
uint16_t warning_year = 0;

class InfoWarningOrder final : public Order {
public:
    void set_callback(void (*)(void)) override {}

    void process() override {}

    void parse(OrderProtocol* socket, uint8_t* data) override {
        (void)socket;
        (void)data;
    }

    uint8_t* build() override {
        const size_t total_size = get_size();
        if (buffer.size() != total_size) {
            buffer.resize(total_size);
        }

        uint8_t* data = buffer.data();
        append(data, &id, sizeof(id));
        append(data, &warning_padding, sizeof(warning_padding));
        append(data, &warning_boundary_type, sizeof(warning_boundary_type));
        append(data, warning_name.c_str(), warning_name.size() + 1);
        append(data, warning_message.c_str(), warning_message.size() + 1);
        append(data, &warning_counter, sizeof(warning_counter));
        append(data, &warning_second, sizeof(warning_second));
        append(data, &warning_minute, sizeof(warning_minute));
        append(data, &warning_hour, sizeof(warning_hour));
        append(data, &warning_day, sizeof(warning_day));
        append(data, &warning_month, sizeof(warning_month));
        append(data, &warning_year, sizeof(warning_year));
        return buffer.data();
    }

    size_t get_size() override {
        size = sizeof(id) + sizeof(warning_padding) + sizeof(warning_boundary_type) +
               warning_name.size() + 1 + warning_message.size() + 1 + sizeof(warning_counter) +
               sizeof(warning_second) + sizeof(warning_minute) + sizeof(warning_hour) +
               sizeof(warning_day) + sizeof(warning_month) + sizeof(warning_year);
        return size;
    }

    uint16_t get_id() override { return id; }

    void set_pointer(size_t index, void* pointer) override {
        (void)index;
        (void)pointer;
    }

private:
    static void append(uint8_t*& dst, const void* src, size_t count) {
        memcpy(dst, src, count);
        dst += count;
    }

    static constexpr uint16_t id = INFO_WARNING_TCP_ORDER_ID;
    vector<uint8_t> buffer{};
};

InfoWarningOrder info_warning_order;

void refresh_warning_transport_state(const string& description) {
    warning_message = description;

#ifdef HAL_RTC_MODULE_ENABLED
    if (Global_RTC::ensure_started()) {
        Global_RTC::update_rtc_data();
        warning_counter = Global_RTC::global_RTC.counter;
        warning_second = Global_RTC::global_RTC.second;
        warning_minute = Global_RTC::global_RTC.minute;
        warning_hour = Global_RTC::global_RTC.hour;
        warning_day = Global_RTC::global_RTC.day;
        warning_month = Global_RTC::global_RTC.month;
        warning_year = Global_RTC::global_RTC.year;
        return;
    }
#endif

    warning_counter = 0;
    warning_second = 0;
    warning_minute = 0;
    warning_hour = 0;
    warning_day = 0;
    warning_month = 0;
    warning_year = 0;
}
#endif

bool try_send_warning_via_tcp(const string& description) {
#ifdef STLIB_ETH
    if (!tcp_delivery_required || warning_sent_via_tcp) {
        return true;
    }

    refresh_warning_transport_state(description);

    bool delivered = false;
    for (OrderProtocol* socket : OrderProtocol::sockets) {
        if (socket == nullptr) {
            continue;
        }
        delivered = socket->send_order(info_warning_order) || delivered;
    }

    if (delivered) {
        warning_sent_via_tcp = true;
    }
    return warning_sent_via_tcp;
#else
    (void)description;
    return true;
#endif
}

bool try_send_warning_via_uart(const string& description) {
#ifdef HAL_UART_MODULE_ENABLED
    if (!uart_delivery_required || warning_sent_via_uart) {
        return true;
    }

    if (!UART::printf_ready) {
        return false;
    }

    printf("Warning: %s%s", description.c_str(), endl);
    warning_sent_via_uart = true;
    return true;
#else
    (void)description;
    return true;
#endif
}

void append_readable_timestamp(string& message) {
#ifdef HAL_RTC_MODULE_ENABLED
    if (Global_RTC::ensure_started() && Global_RTC::has_valid_time()) {
        Global_RTC::update_rtc_data();
        const RTCData& timestamp = Global_RTC::global_RTC;
        char buffer[80]{};
        snprintf(
            buffer,
            sizeof(buffer),
            " | Timestamp: %04u-%02u-%02u %02u:%02u:%02u.%05u",
            static_cast<unsigned>(timestamp.year),
            static_cast<unsigned>(timestamp.month),
            static_cast<unsigned>(timestamp.day),
            static_cast<unsigned>(timestamp.hour),
            static_cast<unsigned>(timestamp.minute),
            static_cast<unsigned>(timestamp.second),
            static_cast<unsigned>(timestamp.counter)
        );
        message += buffer;
        return;
    }
#endif

#ifdef HAL_TIM_MODULE_ENABLED
    const uint64_t uptime_us = Scheduler::get_global_tick();
    const uint64_t total_seconds = uptime_us / 1'000'000ULL;
    const uint64_t days = total_seconds / 86'400ULL;
    const unsigned hours = static_cast<unsigned>((total_seconds / 3'600ULL) % 24ULL);
    const unsigned minutes = static_cast<unsigned>((total_seconds / 60ULL) % 60ULL);
    const unsigned seconds = static_cast<unsigned>(total_seconds % 60ULL);
    const unsigned micros = static_cast<unsigned>(uptime_us % 1'000'000ULL);

    char buffer[80]{};
    if (days > 0) {
        snprintf(
            buffer,
            sizeof(buffer),
            " | Uptime: %llud %02u:%02u:%02u.%06u",
            static_cast<unsigned long long>(days),
            hours,
            minutes,
            seconds,
            micros
        );
    } else {
        snprintf(
            buffer,
            sizeof(buffer),
            " | Uptime: %02u:%02u:%02u.%06u",
            hours,
            minutes,
            seconds,
            micros
        );
    }
    message += buffer;
#endif
}

} // namespace

string InfoWarning::description = "Warning-No-Description-Found";
string InfoWarning::line = "Warning-No-Line-Found";
string InfoWarning::func = "Warning-No-Func-Found";
string InfoWarning::file = "Warning-No-File-Found";
bool InfoWarning::warning_triggered = false;
bool InfoWarning::warning_to_communicate = false;

void InfoWarning::SetMetaData(int line, const char* func, const char* file) {
    InfoWarning::line = to_string(line);
    InfoWarning::func = string(func);
    InfoWarning::file = string(file);
}

void InfoWarning::InfoWarningTrigger(string format, ...) {
    if (InfoWarning::warning_triggered) {
        return;
    }

    InfoWarning::warning_triggered = true;
    InfoWarning::warning_to_communicate = true;
    warning_sent_via_tcp = false;
    warning_sent_via_uart = false;
#ifdef STLIB_ETH
    tcp_delivery_required = true;
#else
    tcp_delivery_required = false;
#endif
#ifdef HAL_UART_MODULE_ENABLED
    uart_delivery_required = UART::printf_ready;
#else
    uart_delivery_required = false;
#endif

    if (format.length() != 0) {
        description = "";
    }

    va_list arguments;
    va_start(arguments, format);
    va_list arg_copy;
    va_copy(arg_copy, arguments);

    const int32_t size = vsnprintf(nullptr, 0, format.c_str(), arguments) + 1;
    const unique_ptr<char[]> buffer = make_unique<char[]>(size);
    va_end(arguments);

    vsnprintf(buffer.get(), size, format.c_str(), arg_copy);
    va_end(arg_copy);

    description += string(buffer.get(), buffer.get() + size - 1) + " | Line: " + InfoWarning::line +
                   " Function: '" + InfoWarning::func + "' File: " + InfoWarning::file;

    append_readable_timestamp(description);

    InfoWarning::InfoWarningUpdate();
}

void InfoWarning::InfoWarningUpdate() {
    if (!InfoWarning::warning_to_communicate) {
        return;
    }

    const bool tcp_done = try_send_warning_via_tcp(InfoWarning::description);
    const bool uart_done = try_send_warning_via_uart(InfoWarning::description);

    if (tcp_done && uart_done) {
        InfoWarning::warning_to_communicate = false;
        InfoWarning::warning_triggered = false;
    }
}
