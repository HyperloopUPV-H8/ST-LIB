#include "Protections/ProtectionManager.hpp"

StateMachine* ProtectionManager::general_state_machine = nullptr;

void ProtectionManager::set_id(int board_id){
    ProtectionManager::board_id = board_id;
}

void ProtectionManager::link_state_machine(StateMachine& general_state_machine, state_id fault_id){
	ProtectionManager::general_state_machine = &general_state_machine;
	ProtectionManager::fault_state_id = fault_id;
}

void ProtectionManager::to_fault(){
    if(general_state_machine->current_state != fault_state_id)
	    ProtectionManager::general_state_machine->force_change_state(fault_state_id);
}

void ProtectionManager::check_protections() {
    for (Protection& protection: protections) {
        if (protection.check_state()) {
            continue;
        }
        if(general_state_machine == nullptr){
        	ErrorHandler("Protection Manager does not have General State Machine Linked");
        	return;
        }

        ProtectionManager::general_state_machine->force_change_state(fault_id);
    }
}

int ProtectionManager::board_id = -1;
vector<Protection> ProtectionManager::protections = {};
