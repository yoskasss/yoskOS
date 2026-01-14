

#include <stdint.h>

uint64_t __udivdi3(uint64_t num, uint64_t den) {
    uint64_t quot = 0, qbit = 1;
    
    if (den == 0) {

        return 0xFFFFFFFFFFFFFFFFULL;
    }
    

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

uint64_t __umoddi3(uint64_t num, uint64_t den) {
    uint64_t quot = __udivdi3(num, den);
    return num - (quot * den);
} 