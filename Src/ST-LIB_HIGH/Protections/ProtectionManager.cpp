#include "Protections/ProtectionManager.hpp"
#include "Time/Time.hpp"
#include "Protections/Notification.hpp"

StateMachine* ProtectionManager::general_state_machine = nullptr;
Notification ProtectionManager::fault_notification = {ProtectionManager::fault_id, ProtectionManager::to_fault};
Notification ProtectionManager::warning_notification = {ProtectionManager::warning_id, nullptr};

void ProtectionManager::set_id(Boards::ID board_id){
	add_protection((void*)nullptr,Boundary<void, ProtectionType::ERROR_HANDLER>());
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
    for (Protection& protection: low_frequency_protections) {
        if (protection.check_state()) {
            continue;
        }
        if(general_state_machine == nullptr){
        	ErrorHandler("Protection Manager does not have General State Machine Linked");
        	return;
        }

        ProtectionManager::to_fault();

        Time::RTCData current_timestamp = Time::get_rtc_data();
        message = (char*)malloc(get_string_size(protection, current_timestamp));
        serialize(protection, current_timestamp);

        switch(protection.fault_type){
        case FaultType::WARNING:
        	warning_notification.notify(message);
        	break;
        case FaultType::FAULT:
        	fault_notification.notify(message);
        	break;
        default:
        	ErrorHandler("Protection has not a Fault Type that can be handled correctly by the ProtectionManager");
        }

    	free(message);
    }
}

void ProtectionManager::check_high_frequency_protections(){
    for (Protection& protection: high_frequency_protections) {
        if (protection.check_state()) {
            continue;
        }
        if(general_state_machine == nullptr){
        	ErrorHandler("Protection Manager does not have General State Machine Linked");
        	return;
        }

        ProtectionManager::to_fault();

        Time::RTCData current_timestamp = Time::get_rtc_data();
        message = (char*)malloc(get_string_size(protection, current_timestamp));
        serialize(protection, current_timestamp);

        switch(protection.fault_type){
        case FaultType::WARNING:
        	warning_notification.notify(message);
        case FaultType::FAULT:
        	fault_notification.notify(message);
        default:
        	ErrorHandler("Protection has not a Fault Type that can be handled correctly by the ProtectionManager");
        }

    	free(message);
    }
}

Boards::ID ProtectionManager::board_id = Boards::ID::NOBOARD;
size_t ProtectionManager::message_size = 0;
char* ProtectionManager::message = nullptr;
ProtectionManager::state_id ProtectionManager::fault_state_id = 255;
vector<Protection> ProtectionManager::low_frequency_protections = {};
vector<Protection> ProtectionManager::high_frequency_protections = {};
