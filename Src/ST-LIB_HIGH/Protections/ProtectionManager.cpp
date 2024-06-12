#include "Protections/ProtectionManager.hpp"
#include "Time/Time.hpp"
#include "Protections/Notification.hpp"

StateMachine* ProtectionManager::general_state_machine = nullptr;
Notification ProtectionManager::fault_notification = {ProtectionManager::fault_id, nullptr};
Notification ProtectionManager::warning_notification = {ProtectionManager::warning_id, nullptr};
StackOrder<0> ProtectionManager::fault_order(Protections::FAULT,to_fault);
uint64_t ProtectionManager::last_notify = 0;
void *error_handler;


void ProtectionManager::initialize(){
    for (Protection& protection: low_frequency_protections) {
        for(auto& boundary : protection.boundaries){
            boundary->update_name(protection.get_name());
        }
    }
    for (Protection& protection: high_frequency_protections) {
        for(auto& boundary : protection.boundaries){
            boundary->update_name(protection.get_name());
        }
    }
}

void ProtectionManager::add_standard_protections(){
	add_protection(error_handler, Boundary<void,ERROR_HANDLER>(error_handler));
}

void ProtectionManager::set_id(Boards::ID board_id){
    ProtectionManager::board_id = board_id;
}

void ProtectionManager::link_state_machine(StateMachine& general_state_machine, state_id fault_id){
	ProtectionManager::general_state_machine = &general_state_machine;
	ProtectionManager::fault_state_id = fault_id;
}

void ProtectionManager::to_fault(){
    if(general_state_machine->current_state != fault_state_id){
	    fault_and_propagate();
	}

}

void ProtectionManager::fault_and_propagate(){
	ProtectionManager::general_state_machine->force_change_state(fault_state_id);
	for(OrderProtocol* socket : OrderProtocol::sockets){
		socket->send_order(fault_order);
	}
}

void ProtectionManager::check_protections() {
    for (Protection& protection: low_frequency_protections) {
        auto protection_status = protection.check_state();

        if(general_state_machine == nullptr){
        	ErrorHandler("Protection Manager does not have General State Machine Linked");
        	return;
        }
        //ensure we only go to FAULT if a FAULT was triggered, and not only a WARNING
        if(protection.fault_type == Protections::FAULT &&
          protection_status == Protections::FAULT){
            ProtectionManager::to_fault();
        }
        Global_RTC::update_rtc_data();
        ProtectionManager::notify(protection);

    }
}

void ProtectionManager::check_high_frequency_protections(){
    for (Protection& protection: high_frequency_protections) {
        if(general_state_machine == nullptr){
        	ErrorHandler("Protection Manager does not have General State Machine Linked");
        	return;
        }

        if(protection.fault_type == Protections::FAULT){
            ProtectionManager::to_fault();
        }
        Global_RTC::update_rtc_data();
        ProtectionManager::notify(protection);
    }
}

void ProtectionManager::warn(string message){
	warning_notification.notify(message);
}

void ProtectionManager::notify(Protection& protection){
    if(protection.fault_protection->boundary_type_id == ERROR_HANDLER){
        protection.fault_protection->update_error_handler_message(protection.fault_protection->get_error_handler_string());
    }
    for(OrderProtocol* socket : OrderProtocol::sockets){
        if(protection.fault_protection)
            socket->send_order(*protection.fault_protection->fault_message);
        for(auto& warning : protection.warnings_triggered){
            if(warning->boundary_type_id == INFO_WARNING){
                warning->update_warning_message(warning->get_warning_string());
                InfoWarning::warning_triggered = false;
            }
            socket->send_order(*warning->warn_message);
        }
        for(auto& ok : protection.oks_triggered){
            socket->send_order(*ok->ok_message);
        }

    }
    protection.oks_triggered.clear();
    protection.warnings_triggered.clear();
}


Boards::ID ProtectionManager::board_id = Boards::ID::NOBOARD;
size_t ProtectionManager::message_size = 0;
char* ProtectionManager::message = nullptr;
ProtectionManager::state_id ProtectionManager::fault_state_id = 255;
vector<Protection> ProtectionManager::low_frequency_protections = {};
vector<Protection> ProtectionManager::high_frequency_protections = {};
