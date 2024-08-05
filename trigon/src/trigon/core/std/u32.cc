#include "std.h"

u32 get_next_power_of_two(u32 number) {
    if (number <= 2) {
        return 2;
    }

    uint32_t num = 2;
    while (number > num) {
        num *= 2;
    }
    return num;
}
