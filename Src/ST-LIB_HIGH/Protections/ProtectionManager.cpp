#include "Protections/ProtectionManager.hpp"

void ProtectionManager::set_id(int board_id){
    ProtectionManager::board_id = board_id;
}
template<class... AdditionalProtections>
Protection& ProtectionManager::_add_protection(double* src, ProtectionType protection_type,double bound, AdditionalProtections... more_protections) {
    protections.push_back(Protection(src));
    protections.back().add_protection(protection_type, bound, more_protections...);
    return protections.back();
}
template<class... AdditionalProtections>
Protection& ProtectionManager::_add_protection(double* src, ProtectionType protection_type, double lower_bound, double upper_bound, AdditionalProtections... more_protections) {
    protections.push_back(Protection(src));
    protections.back().add_protection(protection_type, lower_bound, upper_bound, more_protections...);
    return protections.back();
}

void ProtectionManager::check_protections() {
    for (Protection protection: protections) {
        if (!protection.check_state()) {
        	//TODO: Send StateMachine to FAULT state
            string range = "";
            if (protection.jumped_potection == ProtectionType::OUT_OF_RANGE) {
                range = "[";
                range += (* protection.src < protection.boundaries[protection.boundary_counter - 1]) ? to_string(protection.boundaries[protection.boundary_counter - 1]) + "," + to_string(protection.boundaries[protection.boundary_counter]) + "]" :
                to_string(protection.boundaries[protection.boundary_counter-2]) + "," + to_string(protection.boundaries[protection.boundary_counter-1]) + "]";
            }
            else {
                range = to_string(protection.boundaries[protection.boundary_counter - 1]);
            }
            //TODO: Send Packet through TCP with information
        }
    }
}

int ProtectionManager::board_id = -1;
vector<Protection> ProtectionManager::protections = {};
