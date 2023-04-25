#pragma once

#include "Protection.hpp"
#include "StateMachine/StateMachine.hpp"
#include "Packets/Order.hpp"

#define getname(var) #var
#define add_protection(src,...)  \
            Protection& ref = ProtectionManager::_add_protection(src,__VA_ARGS__); \
            ref.set_name((char*)malloc(sizeof(getname(src))-1)); \
            sprintf(ref.get_name(),"%s",getname(src)+1); \

class ProtectionManager {
public:
	typedef uint8_t state_id;

    static void set_id(int board_id);

    static void link_state_machine(StateMachine& general_state_machine, state_id fault_id);

    template<class Type, ProtectionType... Protector, template<class,ProtectionType> class Boundaries>
    static Protection& _add_protection(Type* src, Boundaries<Type,Protector>... protectors){
        protections.push_back(Protection(src,protectors...));
        return protections.back();
    }

    static void check_protections();

private:
	static constexpr uint16_t warning_id = 1;
	static constexpr uint16_t fault_id = 2;

    static int board_id;
    static vector<Protection> protections;
    static StateMachine* general_state_machine;
    static state_id fault_state_id;

    static void to_fault();
};


