#include "Protections/ProtectionManager.hpp"
#include "Time/Time.hpp"
#include "Protections/Notification.hpp"

StateMachine* ProtectionManager::general_state_machine = nullptr;
Notification ProtectionManager::fault_notification = {ProtectionManager::fault_id, nullptr};
Notification ProtectionManager::warning_notification = {ProtectionManager::warning_id, nullptr};
StackOrder<0> ProtectionManager::fault_order(Protections::FAULT,to_fault);
uint64_t ProtectionManager::last_notify = 0;
void *error_handler;


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
        if (protection.check_state()) {
            continue;
        }
        if(general_state_machine == nullptr){
        	ErrorHandler("Protection Manager does not have General State Machine Linked");
        	return;
        }
        if(protection.fault_type == Protections::FAULT){
            ProtectionManager::to_fault();
        }
        Global_RTC::update_rtc_data();

        
        if(Time::get_global_tick() > last_notify + notify_delay_in_nanoseconds){
            ProtectionManager::notify(protection);
        last_notify = Time::get_global_tick();
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

        Global_RTC::update_rtc_data();
        for(OrderProtocol* socket : OrderProtocol::sockets){
        	socket->send_order(*protection.jumped_protection->message);
        }
        switch(protection.fault_type){
        case Protections::FaultType::WARNING:
        	warning_notification.notify(message);
            break;
        case Protections::FaultType::FAULT:
        	fault_notification.notify(message);
            break;
        default:
        	ErrorHandler("Protection has not a Fault Type that can be handled correctly by the ProtectionManager");
        }

    	free(message);
    }
}

void ProtectionManager::warn(string message){
	warning_notification.notify(message);
}

void ProtectionManager::notify(Protection& protection){

    protection.jumped_protection->update_name(protection.get_name());
    for(OrderProtocol* socket : OrderProtocol::sockets){
        socket->send_order(*protection.jumped_protection->message);
    }
}

Boards::ID ProtectionManager::board_id = Boards::ID::NOBOARD;
size_t ProtectionManager::message_size = 0;
char* ProtectionManager::message = nullptr;
ProtectionManager::state_id ProtectionManager::fault_state_id = 255;
vector<Protection> ProtectionManager::low_frequency_protections = {};
vector<Protection> ProtectionManager::high_frequency_protections = {};
