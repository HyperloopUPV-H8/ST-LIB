#include "HALAL/Services/Diagnostics/Diagnostics.hpp"

#if defined(HAL_UART_MODULE_ENABLED) && !defined(SIM_ON)
#include "HALAL/Services/Communication/UART/UART.hpp"
#endif

#ifdef STLIB_ETH
#include "HALAL/Models/Packets/Order.hpp"
#endif

namespace Diagnostics {

namespace {

constexpr uint16_t DIAGNOSTIC_TCP_ORDER_ID = 3555;
constexpr size_t transport_buffer_capacity = 2 + 1 + 1 + 2 + 1 + 1 + 1 + 1 + 1 + 2 + 8 +
                                             Config::origin_capacity + 1 +
                                             Config::formatted_message_capacity + 1;

size_t bounded_strnlen(const char* src, size_t max_length) {
    if (src == nullptr) {
        return 0;
    }

    size_t length = 0;
    while (length < max_length && src[length] != '\0') {
        length++;
    }
    return length;
}

template <size_t Capacity>
void copy_c_string(char (&dst)[Capacity], const char* src) {
    if (Capacity == 0) {
        return;
    }

    if (src == nullptr) {
        dst[0] = '\0';
        return;
    }

    const size_t length = bounded_strnlen(src, Capacity - 1);
    memcpy(dst, src, length);
    dst[length] = '\0';
}

constexpr uint16_t to_network_u16(uint16_t value) {
    if constexpr (std::endian::native == std::endian::little) {
        return std::byteswap(value);
    }
    return value;
}

constexpr uint64_t to_network_u64(uint64_t value) {
    if constexpr (std::endian::native == std::endian::little) {
        return std::byteswap(value);
    }
    return value;
}

#if defined(HAL_UART_MODULE_ENABLED) && !defined(SIM_ON)
class UartDiagnosticSink final : public DiagnosticSink {
public:
    bool publish(const DiagnosticRecord& record) override {
        if (!UART::printf_ready) {
            return false;
        }

        char description[Config::formatted_message_capacity + 1]{};
        DiagnosticFormatter::describe(record, description, sizeof(description));
        printf("%u: %s%s", std::to_underlying(record.severity), description, endl);
        return true;
    }
};
#endif

#ifdef STLIB_ETH
class DiagnosticTransportOrder final : public Order {
public:
    DiagnosticTransportOrder() : id(DIAGNOSTIC_TCP_ORDER_ID) {
        Packet::packets[id] = this;
        orders[id] = this;
    }

    void set_record(const DiagnosticRecord& record, const char* description) {
        severity = std::to_underlying(record.severity);
        category = std::to_underlying(record.category);
        copy_c_string(origin, record.origin);
        copy_c_string(message, description);
        counter = record.timestamp.counter;
        second = record.timestamp.second;
        minute = record.timestamp.minute;
        hour = record.timestamp.hour;
        day = record.timestamp.day;
        month = record.timestamp.month;
        year = record.timestamp.year;
        uptime_us = record.timestamp.uptime_us;
    }

    void set_callback(void (*callback)(void)) override { this->callback = callback; }

    void process() override {
        if (callback != nullptr) {
            callback();
        }
    }

    void parse(OrderProtocol* socket, uint8_t* data) override {
        (void)socket;
        (void)data;
    }

    uint8_t* build() override {
        auto bytes = span<byte>(buffer.data(), buffer.size());
        size_t offset = 0;

        write(bytes, offset, to_network_u16(id));
        write(bytes, offset, severity);
        write(bytes, offset, category);
        write_string(bytes, offset, origin);
        write_string(bytes, offset, message);
        write(bytes, offset, to_network_u16(counter));
        write(bytes, offset, second);
        write(bytes, offset, minute);
        write(bytes, offset, hour);
        write(bytes, offset, day);
        write(bytes, offset, month);
        write(bytes, offset, to_network_u16(year));
        write(bytes, offset, to_network_u64(uptime_us));

        size = offset;
        return reinterpret_cast<uint8_t*>(buffer.data());
    }

    size_t get_size() override { return size; }

    uint16_t get_id() override { return id; }

    void set_pointer(size_t index, void* pointer) override {
        (void)index;
        (void)pointer;
    }

private:
    template <typename Value> static void write(span<byte> bytes, size_t& offset, Value value) {
        memcpy(bytes.data() + offset, &value, sizeof(Value));
        offset += sizeof(Value);
    }

    static void write_string(span<byte> bytes, size_t& offset, const char* text) {
        const size_t length = bounded_strnlen(text, bytes.size() - offset - 1);
        memcpy(bytes.data() + offset, text, length);
        offset += length;
        bytes[offset++] = byte{0};
    }

    uint16_t id;
    void (*callback)(void) = nullptr;
    uint8_t severity{0};
    uint8_t category{0};
    char origin[Config::origin_capacity + 1]{};
    char message[Config::formatted_message_capacity + 1]{};
    uint16_t counter{0};
    uint8_t second{0};
    uint8_t minute{0};
    uint8_t hour{0};
    uint8_t day{0};
    uint8_t month{0};
    uint16_t year{0};
    uint64_t uptime_us{0};
    size_t size{0};
    array<byte, transport_buffer_capacity> buffer{};
};

class OrderProtocolDiagnosticSink final : public DiagnosticSink {
public:
    bool publish(const DiagnosticRecord& record) override {
        char description[Config::formatted_message_capacity + 1]{};
        DiagnosticFormatter::describe(record, description, sizeof(description));
        transport_order.set_record(record, description);

        bool delivered = false;
        for (OrderProtocol* socket : OrderProtocol::sockets) {
            if (socket == nullptr) {
                continue;
            }
            delivered = socket->send_order(transport_order) || delivered;
        }
        return delivered;
    }

private:
    DiagnosticTransportOrder transport_order{};
};
#endif

} // namespace

void Runtime::install_default_sinks() {
    if (defaults_installed) {
        return;
    }

#if defined(HAL_UART_MODULE_ENABLED) && !defined(SIM_ON)
    (void)Hub::emplace_sink<UartDiagnosticSink>();
#endif

#ifdef STLIB_ETH
    (void)Hub::emplace_sink<OrderProtocolDiagnosticSink>();
#endif

    defaults_installed = true;
}

} // namespace Diagnostics
