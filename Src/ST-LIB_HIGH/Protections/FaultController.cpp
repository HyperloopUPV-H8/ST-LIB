#include "ST-LIB_HIGH/Protections/FaultController.hpp"

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultRuntime.hpp"

IStateMachine* FaultController::general_state_machine = nullptr;
FaultController::state_id FaultController::fault_state_id = 255;
array<FaultController::BroadcasterStorage, FaultController::max_broadcasters>
    FaultController::broadcaster_storage = {};
array<FaultBroadcaster*, FaultController::max_broadcasters> FaultController::broadcasters = {};
size_t FaultController::broadcaster_count = 0;
bool FaultRuntime::defaults_installed = false;

void FaultController::link_state_machine(
    IStateMachine& general_state_machine,
    FaultController::state_id fault_id
) {
    FaultController::general_state_machine = &general_state_machine;
    FaultController::fault_state_id = fault_id;
}

void FaultController::enter_fault() {
    if (general_state_machine == nullptr) {
        Diagnostics::Hub::publish_runtime_error(
            "FaultController does not have General State Machine linked",
            false,
            0,
            "FaultController::enter_fault",
            __FILE__
        );
        return;
    }

    if (general_state_machine->get_current_state_id() == fault_state_id) {
        return;
    }

    general_state_machine->force_change_state(fault_state_id);
    for (size_t broadcaster_index = 0; broadcaster_index < broadcaster_count; broadcaster_index++) {
        if (broadcasters[broadcaster_index] != nullptr) {
            broadcasters[broadcaster_index]->broadcast_fault();
        }
    }
}

void FaultController::enter_external_fault() { enter_fault(); }

void FaultController::clear_broadcasters_for_testing() {
    for (size_t broadcaster_index = 0; broadcaster_index < broadcaster_count; broadcaster_index++) {
        broadcaster_storage[broadcaster_index].reset();
        broadcasters[broadcaster_index] = nullptr;
    }
    broadcaster_count = 0;
    FaultRuntime::reset_for_testing();
}
