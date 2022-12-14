#pragma once

#include "C++Utilities/CppUtils.hpp"

#define PROTECTIONTYPE_LENGTH 5

enum ProtectionType : uint64_t {
    BELOW = std::numeric_limits<uint64_t>::max() - PROTECTIONTYPE_LENGTH + 1,
    ABOVE,
    OUT_OF_RANGE,
    EQUALS,
    NOT_EQUALS
};


class Protection {
public:

    double* src;
    char* name = nullptr;
    vector<ProtectionType> protections;
    vector<double> boundaries;
    uint8_t boundary_counter = 0;
    ProtectionType jumped_potection = (ProtectionType)0;
    static map<ProtectionType, string> protection_type_name;

    Protection(double* src);
    Protection(double* src, ProtectionType protection_type, double boundary);
    Protection(double* src, ProtectionType protection_type, double lower_boundary, double upper_boundary);

    void add_protection(ProtectionType protection_type, double boundary);
    template<class... AdditionalProtections>
    void add_protection(double upper_boundary, ProtectionType protection, AdditionalProtections... protections);
    void add_protection(double last_boundary);
    template<class... AdditionalProtections>
    void add_protection(ProtectionType protection_type, double boundary,AdditionalProtections... protections);

    bool check_state();
};

