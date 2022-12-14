#pragma once

#include "Protection.hpp"

#define getname(var) #var
#define ProtectionManager_add_protection(src,...)  \
            Protection& ref = ProtectionManager::_add_protection(src,__VA_ARGS__); \
            ref.name = (char*)malloc(sizeof(getname(src))-1); \
            sprintf(ref.name,"%s",getname(src)+1); \

class ProtectionManager {
public:
    static int board_id;
    static vector<Protection> protections;

    static void set_id(int board_id);

    template<class... AdditionalProtections>
    static Protection& _add_protection(double* src, ProtectionType protection_type,double bound, AdditionalProtections... more_protections);
    template<class... AdditionalProtections>
    static Protection& _add_protection(double* src, ProtectionType protection_type, double lower_bound, double upper_bound, AdditionalProtections... more_protections);

    static void check_protections();
};