#include "MockedDrivers/compiler_specific.hpp"

uint32_t __CLZ(uint32_t val) {
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
    return __builtin_clz(val);
#elif defined(_MSC_VER)
    DWORD idx = 0;
    _BitScanReverse(&idx, val);
    return 31 - idx;
#else
    for(uint32_t i = 0; i < 32; i++) {
        if((val & (1 << (31 - i))) != 0) return i;
    }
    return 32;
#endif
}
uint32_t __CLZ(uint64_t val) {
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
    return __builtin_clz(val);
#elif defined(_MSC_VER)
    DWORD idx = 0;
    _BitScanReverse64(&idx, val);
    return 63 - idx;
#else
    for(uint32_t i = 0; i < 32; i++) {
        if((val & (1 << (31 - i))) != 0) return i;
    }
    return 32;
#endif
}

uint32_t __CTZ(uint32_t val)
{
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
    return __builtin_ctz(val);
#elif defined(_MSC_VER)
    DWORD idx = 0;
    _BitScanForward(&idx, val);
    return idx;
#else
    for(uint32_t i = 0; i < 32; i++) {
        if((val & (1 << i)) != 0) return i;
    }
    return 32;
#endif
}
uint32_t __CTZ(uint64_t val)
{
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
    return __builtin_ctz(val);
#elif defined(_MSC_VER)
    DWORD idx = 0;
    _BitScanForward64(&idx, val);
    return idx;
#else
    for(uint32_t i = 0; i < 64; i++) {
        if((val & (1 << i)) != 0) return i;
    }
    return 64;
#endif
}