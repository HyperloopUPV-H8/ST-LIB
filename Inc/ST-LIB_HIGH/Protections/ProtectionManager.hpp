#pragma once

#include "Protection.hpp"
#include "StateMachine/StateMachine.hpp"
#include "Packets/Order.hpp"
#include "Time/Time.hpp"
#include "Notification.hpp"
#include "BoardID/BoardID.hpp"

#define getname(var) #var
#define add_protection(src,...)  \
		{\
            Protection& ref = ProtectionManager::_add_protection(src,__VA_ARGS__); \
            ref.set_name((char*)malloc(sizeof(getname(src))-1)); \
            sprintf(ref.get_name(),"%s",getname(src)+1); \
		}\

#define add_high_frequency_protection(src,...)  \
		{\
            Protection& ref = ProtectionManager::_add_high_frequency_protection(src,__VA_ARGS__); \
            ref.set_name((char*)malloc(sizeof(getname(src))-1)); \
            sprintf(ref.get_name(),"%s",getname(src)+1); \
		}\

class ProtectionManager {
public:
	typedef uint8_t state_id;

    static void set_id(BoardID id);

    static void link_state_machine(StateMachine& general_state_machine, state_id fault_id);

    template<class Type, ProtectionType... Protector, template<class,ProtectionType> class Boundaries>
    static Protection& _add_protection(Type* src, Boundaries<Type,Protector>... protectors){
    	low_frequency_protections.push_back(Protection(src,protectors...));
        return low_frequency_protections.back();
    }

    template<class Type, ProtectionType... Protector, template<class,ProtectionType> class Boundaries>
    static Protection& _add_high_frequency_protection(Type* src, Boundaries<Type,Protector>... protectors){
    	high_frequency_protections.push_back(Protection(src,protectors...));
        return high_frequency_protections.back();
    }

    static void check_protections();
    static void check_high_frequency_protections();

private:
	static constexpr uint16_t warning_id = 1;
	static constexpr uint16_t fault_id = 2;
	static char* message;
	static size_t message_size;
	static constexpr const char* format = "{\"boardId\": %s, \"timestamp\":{%s}, %s}";

    static BoardID board_id;
    static vector<Protection> low_frequency_protections;
    static vector<Protection> high_frequency_protections;
    static StateMachine* general_state_machine;
    static state_id fault_state_id;

    static Notification fault_notification;
    static Notification warning_notification;

    static int get_string_size(Protection& prot ,const Time::RTCData& timestamp){
    	return snprintf(nullptr,0,format,"","","") + prot.get_string_size() + Time::RTCData::get_string_size(timestamp);
    }

    static char* serialize(Protection& prot, const Time::RTCData& timestamp){
    	sprintf(message,format,to_string(board_id).c_str(),timestamp.serialize().c_str(),string(prot.serialize(message)).c_str());
    	return message;
    }

    static void to_fault();
};


