#include "Protections/Protection.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

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

map<ProtectionType, string> Protection<>::protection_type_name = { {ProtectionType::BELOW,"minimo"},
                                                                 {ProtectionType::ABOVE,"maximo"},
                                                                 {ProtectionType::EQUALS, "deberia ser distinto al"},
                                                                 {ProtectionType::NOT_EQUALS, "deberia ser igual al"},
                                                                 {ProtectionType::OUT_OF_RANGE, "minimo y maximo"}
};
