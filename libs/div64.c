// 64-bit division functions for freestanding environment
// These are needed for 64-bit arithmetic operations

#include <stdint.h>

// Unsigned 64-bit division
uint64_t __udivdi3(uint64_t num, uint64_t den) {
    uint64_t quot = 0, qbit = 1;
    
    if (den == 0) {
        // Division by zero - return maximum value
        return 0xFFFFFFFFFFFFFFFFULL;
    }
    
    // Left-justify denominator and count shift
    while ((int64_t)den >= 0) {
        den <<= 1;
        qbit <<= 1;
    }
    
    while (qbit) {
        if (den <= num) {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }
    
    return quot;
}

// Unsigned 64-bit modulo
uint64_t __umoddi3(uint64_t num, uint64_t den) {
    uint64_t quot = __udivdi3(num, den);
    return num - (quot * den);
} 