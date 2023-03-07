#pragma once

#include "Protection.hpp"
#include "ST-LIB_LOW/ST-LIB_LOW.hpp"

#define getname(var) #var
#define ProtectionManager_add_protection(src,...)  \
            Protection& ref = ProtectionManager::_add_protection(src,__VA_ARGS__); \
            ref.name = (char*)malloc(sizeof(getname(src))-1); \
            sprintf(ref.name,"%s",getname(src)+1); \

class ProtectionManager {
public:
    static int board_id;
    static vector<Protection> protections;
    static StateMachine state_machine;
    static uint8_t faulty_state;
    static Socket& socket;

    static void set_id(int board_id);

    template<class... AdditionalProtections>
    static Protection& _add_protection(double* src, ProtectionType protection_type,double bound, AdditionalProtections... more_protections);
    template<class... AdditionalProtections>
    static Protection& _add_protection(double* src, ProtectionType protection_type, double lower_bound, double upper_bound, AdditionalProtections... more_protections);

    static void set_state_machine(StateMachine& state_machine, uint8_t faulty_state);
    static void set_tcp_socket(Socket& socket);
    static void check_protections();
};

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
