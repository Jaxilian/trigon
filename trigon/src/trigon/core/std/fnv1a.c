#include "std.h"

#define FNV_PRIME_32 16777619
#define FNV_OFFSET_32 2166136261U

u32 fnv1a_hash(cstr_t text) {
    u32 hash = FNV_OFFSET_32;
    for (const char* ptr = text; *ptr; ++ptr) {
        hash ^= (u32)(*ptr); 
        hash *= FNV_PRIME_32;   
    }
    return hash;
}