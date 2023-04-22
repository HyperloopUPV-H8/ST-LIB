#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

#define PROTECTIONTYPE_LENGTH 5

enum ProtectionType : uint64_t {
    BELOW = std::numeric_limits<uint64_t>::max() - PROTECTIONTYPE_LENGTH + 1,
    ABOVE,
    OUT_OF_RANGE,
    EQUALS,
    NOT_EQUALS
};

template<class... Types> class Protection;

template<>
class Protection<>{
public:
	Protection() = default;
    char* name = nullptr;
    static map<ProtectionType, string> protection_type_name;

    virtual bool check_state() = 0;
};

template<class Type>
class Protection<Type> : Protection<> {
public:
    Type* src;
    vector<ProtectionType> protections;
    vector<Type> boundaries;
    uint8_t boundary_counter = 0;
    ProtectionType jumped_protection = (ProtectionType)0;

    Protection(Type* src);
    Protection(Type* src, ProtectionType protection_type, Type boundary);
    Protection(Type* src, ProtectionType protection_type, Type lower_boundary, Type upper_boundary);

    void add_protection(ProtectionType protection_type, Type boundary);
    template<class... AdditionalProtections>
    void add_protection(Type upper_boundary, ProtectionType protection, AdditionalProtections...);
    void add_protection(Type last_boundary);
    template<class... AdditionalProtections>
    void add_protection(ProtectionType protection_type, Type boundary, AdditionalProtections...);

    bool check_state();
};

template<class Type>
template<class... AdditionalProtections>
void Protection<Type>::add_protection(Type upper_boundary, ProtectionType protection, AdditionalProtections... rest) {
    boundaries.push_back(upper_boundary);
    add_protection(protection, rest...);
}

template<class Type>
template<class... AdditionalProtections>
void Protection<Type>::add_protection(ProtectionType protection_type, Type boundary,AdditionalProtections... protections) {
    add_protection(protection_type, boundary);
    add_protection(protections...);
}
