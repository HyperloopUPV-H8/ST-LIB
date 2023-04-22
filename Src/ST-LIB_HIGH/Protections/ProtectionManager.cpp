#include "Protections/ProtectionManager.hpp"

void ProtectionManager::set_id(int board_id){
    ProtectionManager::board_id = board_id;
}

void ProtectionManager::check_protections() {
    for (unique_ptr<Protection<>>& protection: protections) {
        if (protection->check_state()) {
            continue;
        }
//
//        //TODO: Send StateMachine to FAULT state
//        string range = "";
//        string penultimate_protection = to_string(protection->boundaries[protection->boundary_counter - 1]);
//        if (protection->jumped_protection != ProtectionType::OUT_OF_RANGE) {
//            range = penultimate_protection;
//            //TODO: Send Packet through TCP with information
//            continue;
//        }
//
//        string last_protection = to_string(protection->boundaries[protection->boundary_counter]);
//        string antepenultimate_protection = to_string(protection->boundaries[protection->boundary_counter - 2]);
//        if (*protection->src < protection->boundaries[protection->boundary_counter - 1]) {
//            range = "[" + penultimate_protection + "," + last_protection + "]";
//        }
//        else {
//            range = "[" + antepenultimate_protection + "," + penultimate_protection + "]";
//        }
//        //TODO: Send Packet through TCP with information
    }
}

int ProtectionManager::board_id = -1;
vector<unique_ptr<Protection<>>> ProtectionManager::protections = {};
