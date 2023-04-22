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

template<class Type>
Protection<Type>::Protection(Type* src): src(src) {}

template<class Type>
Protection<Type>::Protection(Type* src, ProtectionType protection_type, Type boundary) :src(src) {
    protections.push_back(protection_type);
    boundaries.push_back(boundary);
}

template<class Type>
Protection<Type>::Protection(Type* src, ProtectionType protection_type, Type lower_boundary, Type upper_boundary) :src(src) {
    if (protection_type != OUT_OF_RANGE) {
    	ErrorHandler("You can not use two values for this protection");
        return;
    }
    protections.push_back(protection_type);
    boundaries.push_back(lower_boundary);
    boundaries.push_back(upper_boundary);
}

template<class Type>
void Protection<Type>::add_protection(ProtectionType protection_type, Type boundary) {
    protections.push_back(protection_type);
    boundaries.push_back(boundary);
}

template<class Type>
void Protection<Type>::add_protection(Type last_boundary) {
    boundaries.push_back(last_boundary);
}

template<class Type>
bool Protection<Type>::check_state() {
    for (ProtectionType protection : protections) {
        if (boundary_counter + 1 > (int)boundaries.size()) {
            return false;//TODO: Error Handler
        }
        switch (protection) {
        case BELOW:
            if (*src < boundaries[boundary_counter++]) {
                jumped_protection = protection;
                return false;
            }
            break;
        case ABOVE:
            if (*src > boundaries[boundary_counter++]) {
                jumped_protection = protection;
                return false;
            }
            break;
        case OUT_OF_RANGE:
            if (*src < boundaries[boundary_counter++]) {
                jumped_protection = protection;
                return false;
            }
            if (*src > boundaries[boundary_counter++]) {
                jumped_protection = protection;
                return false;
            }
            break;
        case EQUALS:
            if (*src == boundaries[boundary_counter++]) {
                jumped_protection = protection;
                return false;
            }
            break;
        case NOT_EQUALS:
            if (*src != boundaries[boundary_counter++]) {
                jumped_protection = protection;
                return false;
            }
            break;
        default:
            break;
        }

    }
    boundary_counter = 0;
    return true;
}

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
