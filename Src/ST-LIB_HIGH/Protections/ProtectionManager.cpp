#include "Protections/ProtectionManager.hpp"

void ProtectionManager::set_id(int board_id){
    ProtectionManager::board_id = board_id;
}

void ProtectionManager::set_state_machine(StateMachine& state_machine, uint8_t faulty_state){
	ProtectionManager::state_machine = state_machine;
	ProtectionManager::faulty_state = faulty_state;
}

void ProtectionManager::set_tcp_server_socket(Socket& socket){
	ProtectionManager::socket = socket;
}

void ProtectionManager::check_protections() {
    for (Protection& protection: protections) {
    	if(!protection.check_state()){
    		state_machine.force_change_state(faulty_state);

    	}
    }
}

Socket ProtectionManager::socket = {};
StateMachine ProtectionManager::state_machine = {};
uint8_t ProtectionManager::faulty_state = 255;
int ProtectionManager::board_id = -1;
vector<Protection> ProtectionManager::protections = {};
