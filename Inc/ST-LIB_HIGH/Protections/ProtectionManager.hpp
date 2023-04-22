#pragma once

#include "Protection.hpp"

#define getname(var) #var
#define ProtectionManager_add_protection(src,...)  \
            Protection<>& ref = ProtectionManager::_add_protection(src,__VA_ARGS__); \
            ref.name = (char*)malloc(sizeof(getname(src))-1); \
            sprintf(ref.name,"%s",getname(src)+1); \

class ProtectionManager {
public:
    static int board_id;
    static vector<unique_ptr<Protection<>>> protections;

    static void set_id(int board_id);

    template<class Type, class... AdditionalProtections> requires requires {!is_same<Type,ProtectionType>::value;}
    static Protection<>& _add_protection(Type* src, ProtectionType protection_type,Type bound, AdditionalProtections... more_protections);
    template<class Type, class... AdditionalProtections> requires requires {!is_same<Type,ProtectionType>::value;}
    static Protection<>& _add_protection(Type* src, ProtectionType protection_type, Type lower_bound, Type upper_bound, AdditionalProtections... more_protections);

    static void check_protections();
};

template<class Type, class... AdditionalProtections> requires requires {!is_same<Type,ProtectionType>::value;}
Protection<>& ProtectionManager::_add_protection(Type* src, ProtectionType protection_type,Type bound, AdditionalProtections... more_protections) {
	Protection<Type>* new_protection = new Protection<Type>(src);
	new_protection->add_protection(protection_type, bound, more_protections...);
    protections.push_back(unique_ptr<Protection<>>(new_protection));
    return *protections.back().get();
}

template<class Type, class... AdditionalProtections> requires requires{!is_same<Type,ProtectionType>::value;}
Protection<>& ProtectionManager::_add_protection(Type* src, ProtectionType protection_type, Type lower_bound, Type upper_bound, AdditionalProtections... more_protections) {
	Protection<Type>* new_protection = new Protection<Type>(src);
	new_protection->add_protection(protection_type, lower_bound, upper_bound, more_protections...);
	protections.push_back(unique_ptr<Protection<>>(new_protection));
    return *protections.back().get();
}
