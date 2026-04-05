#include "ST-LIB_HIGH/Protections/FaultRuntime.hpp"

#include "ST-LIB_HIGH/Protections/FaultController.hpp"

#include "HALAL/Models/Packets/Order.hpp"
#include "HALAL/Services/Communication/FDCAN/FDCAN.hpp"

namespace {

constexpr uint16_t remote_fault_order_id = 0;

void handle_remote_fault() { FaultController::enter_external_fault(); }

StackOrder<0>& fault_order() {
    static StackOrder<0> order(remote_fault_order_id, handle_remote_fault);
    return order;
}

#if defined(HAL_FDCAN_MODULE_ENABLED) && !defined(SIM_ON)
class FdcanFaultBroadcaster final : public FaultBroadcaster {
public:
    bool broadcast_fault() override {
        bool delivered = false;
        for (const auto& [key, value] : FDCAN::registered_fdcan) {
            (void)value;
            delivered = FDCAN::transmit(key, FDCAN::ID::FAULT_ID, NULL) || delivered;
        }
        return delivered;
    }
};
#endif

#ifdef STLIB_ETH
class OrderProtocolFaultBroadcaster final : public FaultBroadcaster {
public:
    bool broadcast_fault() override {
        bool delivered = false;
        for (OrderProtocol* socket : OrderProtocol::sockets) {
            if (socket == nullptr) {
                continue;
            }
            delivered = socket->send_order(fault_order()) || delivered;
        }
        return delivered;
    }
};
#endif

} // namespace

void FaultRuntime::install_default_broadcasters() {
    if (defaults_installed) {
        return;
    }

#if defined(HAL_FDCAN_MODULE_ENABLED) && !defined(SIM_ON)
    (void)FaultController::emplace_broadcaster<FdcanFaultBroadcaster>();
#endif

#ifdef STLIB_ETH
    (void)fault_order();
    (void)FaultController::emplace_broadcaster<OrderProtocolFaultBroadcaster>();
#endif

    defaults_installed = true;
}

void FaultRuntime::reset_for_testing() { defaults_installed = false; }
