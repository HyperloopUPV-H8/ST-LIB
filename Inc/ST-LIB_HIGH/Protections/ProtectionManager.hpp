#pragma once

#include "Protection.hpp"
#include "StateMachine/StateMachine.hpp"
#include "Packets/Order.hpp"
#include "Time/Time.hpp"

#define getname(var) #var
#define add_protection(src,...)  \
		{\
            Protection& ref = ProtectionManager::_add_protection(src,__VA_ARGS__); \
            ref.set_name((char*)malloc(sizeof(getname(src))-1)); \
            sprintf(ref.get_name(),"%s",getname(src)+1); \
		}\

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
	static char* message;
	static size_t message_size;
	static constexpr const char* format = "{\"boardId\": %s, \"timestamp\":{%s}, %s}";

    static int board_id;
    static vector<Protection> protections;
    static StateMachine* general_state_machine;
    static state_id fault_state_id;

    static int get_string_size(Protection& prot){
    	return snprintf(nullptr,0,format,"","","") + prot.get_string_size();
    }

    static char* serialize(Protection& prot){
    	message_size = get_string_size(prot);
    	if(message != nullptr) delete[] message;
    	message = new char[message_size];
    	sprintf(message,format,to_string(board_id).c_str(),Time::get_rtc_data().serialize().c_str(),prot.serialize(message));
    	return message;
    }

    static void to_fault();
};


