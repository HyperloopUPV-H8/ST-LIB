/*
 * ErrorHandler.cpp
 *
 *  Created on: Dec 22, 2022
 *      Author: Pablo
 */

#include "ErrorHandler/ErrorHandler.hpp"
#include "HALAL/Services/Time/Scheduler.hpp"
#include "HALAL/Models/Packets/Order.hpp"
#include "HALAL/Services/Time/RTC.hpp"

namespace {

constexpr uint16_t ERROR_HANDLER_TCP_ORDER_ID = 1555;
constexpr uint8_t ERROR_HANDLER_BOUNDARY_TYPE_ID = 5;

bool error_sent_via_tcp = false;
bool error_sent_via_uart = false;
bool tcp_delivery_required = false;
bool uart_delivery_required = false;

uint8_t error_handler_padding = 0;
uint8_t error_handler_boundary_type = ERROR_HANDLER_BOUNDARY_TYPE_ID;
string error_handler_name = "error_handler";
string error_handler_message = "Error-No-Description-Found";
uint16_t error_handler_counter = 0;
uint8_t error_handler_second = 0;
uint8_t error_handler_minute = 0;
uint8_t error_handler_hour = 0;
uint8_t error_handler_day = 0;
uint8_t error_handler_month = 0;
uint16_t error_handler_year = 0;

class ErrorHandlerOrder final : public Order {
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
        append(data, &error_handler_padding, sizeof(error_handler_padding));
        append(data, &error_handler_boundary_type, sizeof(error_handler_boundary_type));
        append(data, error_handler_name.c_str(), error_handler_name.size() + 1);
        append(data, error_handler_message.c_str(), error_handler_message.size() + 1);
        append(data, &error_handler_counter, sizeof(error_handler_counter));
        append(data, &error_handler_second, sizeof(error_handler_second));
        append(data, &error_handler_minute, sizeof(error_handler_minute));
        append(data, &error_handler_hour, sizeof(error_handler_hour));
        append(data, &error_handler_day, sizeof(error_handler_day));
        append(data, &error_handler_month, sizeof(error_handler_month));
        append(data, &error_handler_year, sizeof(error_handler_year));
        return buffer.data();
    }

    size_t get_size() override {
        size = sizeof(id) + sizeof(error_handler_padding) + sizeof(error_handler_boundary_type) +
               error_handler_name.size() + 1 + error_handler_message.size() + 1 +
               sizeof(error_handler_counter) + sizeof(error_handler_second) +
               sizeof(error_handler_minute) + sizeof(error_handler_hour) +
               sizeof(error_handler_day) + sizeof(error_handler_month) + sizeof(error_handler_year);
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

    static constexpr uint16_t id = ERROR_HANDLER_TCP_ORDER_ID;
    vector<uint8_t> buffer{};
};

ErrorHandlerOrder error_handler_order;

void refresh_error_handler_transport_state(const string& description) {
    error_handler_message = description;

#ifdef HAL_RTC_MODULE_ENABLED
    if (Global_RTC::ensure_started()) {
        Global_RTC::update_rtc_data();
        error_handler_counter = Global_RTC::global_RTC.counter;
        error_handler_second = Global_RTC::global_RTC.second;
        error_handler_minute = Global_RTC::global_RTC.minute;
        error_handler_hour = Global_RTC::global_RTC.hour;
        error_handler_day = Global_RTC::global_RTC.day;
        error_handler_month = Global_RTC::global_RTC.month;
        error_handler_year = Global_RTC::global_RTC.year;
        return;
    }
#endif

    error_handler_counter = 0;
    error_handler_second = 0;
    error_handler_minute = 0;
    error_handler_hour = 0;
    error_handler_day = 0;
    error_handler_month = 0;
    error_handler_year = 0;
}

bool try_send_error_via_tcp(const string& description) {
#ifdef STLIB_ETH
    if (!tcp_delivery_required || error_sent_via_tcp) {
        return true;
    }

    refresh_error_handler_transport_state(description);

    bool delivered = false;
    for (OrderProtocol* socket : OrderProtocol::sockets) {
        if (socket == nullptr) {
            continue;
        }
        delivered = socket->send_order(error_handler_order) || delivered;
    }

    if (delivered) {
        error_sent_via_tcp = true;
    }
    return error_sent_via_tcp;
#else
    (void)description;
    return true;
#endif
}

bool try_send_error_via_uart(const string& description) {
#ifdef HAL_UART_MODULE_ENABLED
    if (!uart_delivery_required || error_sent_via_uart) {
        return true;
    }

    if (!UART::printf_ready) {
        return false;
    }

    printf("Error: %s%s", description.c_str(), endl);
    error_sent_via_uart = true;
    return true;
#else
    (void)description;
    return true;
#endif
}

} // namespace

string ErrorHandlerModel::description = "Error-No-Description-Found";
string ErrorHandlerModel::line = "Error-No-Line-Found";
string ErrorHandlerModel::func = "Error-No-Func-Found";
string ErrorHandlerModel::file = "Error-No-File-Found";
double ErrorHandlerModel::error_triggered = 0;
bool ErrorHandlerModel::error_to_communicate = false;

void ErrorHandlerModel::SetMetaData(int line, const char* func, const char* file) {
    ErrorHandlerModel::line = to_string(line);
    ErrorHandlerModel::func = string(func);
    ErrorHandlerModel::file = string(file);
}

void ErrorHandlerModel::ErrorHandlerTrigger(string format, ...) {
    if (ErrorHandlerModel::error_triggered) {
        return;
    }

    ErrorHandlerModel::error_triggered = 1.0;
    ErrorHandlerModel::error_to_communicate =
        true; // This flag is marked so the ProtectionManager can know if it already consumed the
              // error in question.
    error_sent_via_tcp = false;
    error_sent_via_uart = false;
#ifdef STLIB_ETH
    tcp_delivery_required = !OrderProtocol::sockets.empty();
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

    description += string(buffer.get(), buffer.get() + size - 1) +
                   " | Line: " + ErrorHandlerModel::line + " Function: '" +
                   ErrorHandlerModel::func + "' File: " + ErrorHandlerModel::file;

#ifdef HAL_TIM_MODULE_ENABLED
    description += " | TimeStamp: " + to_string(Scheduler::get_global_tick());
#endif

    ErrorHandlerModel::ErrorHandlerUpdate();
}

void ErrorHandlerModel::ErrorHandlerUpdate() {
    if (!ErrorHandlerModel::error_triggered || !ErrorHandlerModel::error_to_communicate) {
        return;
    }

    const bool tcp_done = try_send_error_via_tcp(ErrorHandlerModel::description);
    const bool uart_done = try_send_error_via_uart(ErrorHandlerModel::description);

    if (tcp_done && uart_done) {
        ErrorHandlerModel::error_to_communicate = false;
    }
}
