#pragma once
#include <stdint.h>

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* _BitScanForward, _BitScanReverse */
#include <stdlib.h> /* _byteswap_ulong */
#endif

/*
 * This file contains implementations or alternate names for
 * ARM GCC intrinsics that don't work on x86_64 / host platforms.
 */

//////////////////////////////////////////////////////////////////

#ifdef __COMPILER_BARRIER
#undef __COMPILER_BARRIER
#endif

//////////////////////////////////////////////////////////////////

static inline uint32_t __RBIT(uint32_t val) {
    // 1. Hardware Byte Swap (Optimization: handles the large movements)
    // MSVC uses _byteswap_ulong, GCC/Clang uses __builtin_bswap32
#if defined(_MSC_VER)
    val = _byteswap_ulong(val);
#else
    val = __builtin_bswap32(val);
#endif

    // 2. Swap Nibbles (within bytes)
    // 0xF0 = 1111 0000 -> shifts to 0000 1111
    val = ((val & 0xF0F0F0F0u) >> 4) | ((val & 0x0F0F0F0Fu) << 4);

    // 3. Swap Bit-Pairs (within nibbles)
    // 0xCC = 1100 1100 -> shifts to 0011 0011
    val = ((val & 0xCCCCCCCCu) >> 2) | ((val & 0x33333333u) << 2);

    // 4. Swap Single Bits (within pairs)
    // 0xAA = 1010 1010 -> shifts to 0101 0101
    val = ((val & 0xAAAAAAAAu) >> 1) | ((val & 0x55555555u) << 1);

    return val;
}

inline uint32_t __CLZ(uint32_t val) {
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

inline uint32_t __CLZ(uint64_t val) {
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

inline uint32_t __CTZ(uint32_t val)
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

inline uint32_t __CTZ(uint64_t val)
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

#if defined(_MSC_VER) && (_MSC_VER > 1200) && !defined(__clang__)
void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
#define __COMPILER_BARRIER() _ReadWriteBarrier()
#define __DSB() __COMPILER_BARRIER()
#define __ISB() __COMPILER_BARRIER()
#else

#define __COMPILER_BARRIER() asm volatile("" ::: "memory")

// Architecture-specific definitions for barrier intrinsics used in mocks
#if defined(__x86_64__) || defined(_M_X64)

// Host x86_64
#define __DSB() __asm__ volatile("mfence" ::: "memory")
#define __ISB() __asm__ volatile("lfence" ::: "memory")

#elif defined(__aarch64__) || defined(_M_ARM64)

// Host ARM64
#define __DSB() __asm__ volatile("dmb ish" ::: "memory")
#define __ISB() __asm__ volatile("isb" ::: "memory")

#else

// Any other host architecture: compiler barrier only
#define __DSB() __COMPILER_BARRIER()
#define __ISB() __COMPILER_BARRIER()

#endif
#endif
