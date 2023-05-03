#include "Protections/ProtectionManager.hpp"
#include "Time/Time.hpp"
#include "Protections/Notification.hpp"

StateMachine* ProtectionManager::general_state_machine = nullptr;
Notification ProtectionManager::fault_notification = {ProtectionManager::fault_id, ProtectionManager::to_fault};
Notification ProtectionManager::warning_notification = {ProtectionManager::warning_id, nullptr};

void ProtectionManager::set_id(BoardID board_id){
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

        ProtectionManager::to_fault();

        if(protection.fault_type != FaultType::FAULT) {
        	char* message = (char*)malloc(protection.get_string_size());
        	warning_notification.notify(string(protection.serialize(message)));
        	free(message);
        }

    	char* message = (char*)malloc(protection.get_string_size());
    	fault_notification.notify(string(protection.serialize(message)));
    	free(message);

    }
}

BoardID ProtectionManager::board_id = NOBOARD;
size_t ProtectionManager::message_size = 0;
char* ProtectionManager::message = nullptr;
ProtectionManager::state_id ProtectionManager::fault_state_id = 255;
vector<Protection> ProtectionManager::protections = {};
