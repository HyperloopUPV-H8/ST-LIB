//#include "Protections/ProtectionManager.hpp"
//
//StateMachine* ProtectionManager::general_state_machine = nullptr;
//
//void ProtectionManager::set_id(int board_id){
//    ProtectionManager::board_id = board_id;
//}
//
//void ProtectionManager::link_state_machine(StateMachine& general_state_machine, state_id fault_id){
//	ProtectionManager::general_state_machine = &general_state_machine;
//	ProtectionManager::fault_state_id = fault_id;
//}
//
//void ProtectionManager::to_fault(){
//	ProtectionManager::general_state_machine->force_change_state(fault_state_id);
//}
//
//StackOrder<0> ProtectionManager::fault_order = {
//		ProtectionManager::fault_id,
//		ProtectionManager::to_fault,
//		&ProtectionManager::board_id,
//
//};
//
//StackOrder<0> ProtectionManager::warning_order = {
//		ProtectionManager::warning_id,
//};
//
//void ProtectionManager::check_protections() {
//    for (unique_ptr<Protection<>>& protection: protections) {
//        if (protection->check_state()) {
//            continue;
//        }
//        if(general_state_machine == nullptr){
//        	ErrorHandler("Protection Manager does not have General State Machine Linked");
//        	return;
//        }
//
//        ProtectionManager::general_state_machine->force_change_state(fault_id);
////        string range = "";
////        string penultimate_protection = to_string(protection->boundaries[protection->boundary_counter - 1]);
////        if (protection->jumped_protection != ProtectionType::OUT_OF_RANGE) {
////            range = penultimate_protection;
////            //TODO: Send Packet through TCP with information
////            continue;
////        }
////
////        string last_protection = to_string(protection->boundaries[protection->boundary_counter]);
////        string antepenultimate_protection = to_string(protection->boundaries[protection->boundary_counter - 2]);
////        if (*protection->src < protection->boundaries[protection->boundary_counter - 1]) {
////            range = "[" + penultimate_protection + "," + last_protection + "]";
////        }
////        else {
////            range = "[" + antepenultimate_protection + "," + penultimate_protection + "]";
////        }
////        //TODO: Send Packet through TCP with information
//    }
//}
//
//int ProtectionManager::board_id = -1;
//vector<unique_ptr<Protection<>>> ProtectionManager::protections = {};
