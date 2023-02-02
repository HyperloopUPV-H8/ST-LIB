#include "Protections/Protection.hpp"

Protection::Protection(double* src): src(src) {}

Protection::Protection(double* src, ProtectionType protection_type, double boundary) :src(src) {
    protections.push_back(protection_type);
    boundaries.push_back(boundary);
}

Protection::Protection(double* src, ProtectionType protection_type, double lower_boundary, double upper_boundary) :src(src) {
    if (protection_type != OUT_OF_RANGE) {
    	printf("You can not use two values for this protection");
        return;
    }
    protections.push_back(protection_type);
    boundaries.push_back(lower_boundary);
    boundaries.push_back(upper_boundary);
}

void Protection::add_protection(ProtectionType protection_type, double boundary) {
    protections.push_back(protection_type);
    boundaries.push_back(boundary);
}

void Protection::add_protection(double last_boundary) {
    boundaries.push_back(last_boundary);
}

bool Protection::check_state() {
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

map<ProtectionType, string> Protection::protection_type_name = { {ProtectionType::BELOW,"minimo"},
                                                                 {ProtectionType::ABOVE,"maximo"},
                                                                 {ProtectionType::EQUALS, "deberia ser distinto al"},
                                                                 {ProtectionType::NOT_EQUALS, "deberia ser igual al"},
                                                                 {ProtectionType::OUT_OF_RANGE, "minimo y maximo"}
};
