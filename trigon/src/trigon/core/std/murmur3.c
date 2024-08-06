#include "std.h"


u32 murmur3_32(const u8* key, u64 len, u32 seed) {
    u32 h = seed;
    if (len > 3) {
        const u32* key_x4 = (const u32*)key;
        size_t i = len >> 2;
        do {
            u32 k = *key_x4++;
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;

            h ^= k;
            h = (h << 13) | (h >> 19);
            h = h * 5 + 0xe6546b64;
        } while (--i);
        key = (const u8*)key_x4;
    }

    if (len & 3) {
        size_t i = len & 3;
        u32 k = 0;
        key = &key[i - 1];
        do {
            k <<= 8;
            k |= *key--;
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }

    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

u64 murmur3_64(const u8* key, u64 len, u32 seed) {
    u64 h1 = seed;
    u64 h2 = seed;

    const u64 c1 = 0x87c37b91114253d5ULL;
    const u64 c2 = 0x4cf5ad432745937fULL;

    const u64* key_x8 = (const u64*)key;
    const u8* tail = key + (len & ~0xF);

    size_t nblocks = len / 16;
    for (size_t i = 0; i < nblocks; i++) {
        u64 k1 = key_x8[2 * i + 0];
        u64 k2 = key_x8[2 * i + 1];

        k1 *= c1;
        k1 = (k1 << 31) | (k1 >> 33);
        k1 *= c2;
        h1 ^= k1;

        h1 = (h1 << 27) | (h1 >> 37);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = (k2 << 33) | (k2 >> 31);
        k2 *= c1;
        h2 ^= k2;

        h2 = (h2 << 31) | (h2 >> 33);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    u64 k1 = 0;
    u64 k2 = 0;

    switch (len & 15) {
    case 15: k2 ^= ((u64)tail[14]) << 48;
    case 14: k2 ^= ((u64)tail[13]) << 40;
    case 13: k2 ^= ((u64)tail[12]) << 32;
    case 12: k2 ^= ((u64)tail[11]) << 24;
    case 11: k2 ^= ((u64)tail[10]) << 16;
    case 10: k2 ^= ((u64)tail[9]) << 8;
    case 9: k2 ^= ((u64)tail[8]) << 0;
        k2 *= c2;
        k2 = (k2 << 33) | (k2 >> 31);
        k2 *= c1;
        h2 ^= k2;

    case 8: k1 ^= ((u64)tail[7]) << 56;
    case 7: k1 ^= ((u64)tail[6]) << 48;
    case 6: k1 ^= ((u64)tail[5]) << 40;
    case 5: k1 ^= ((u64)tail[4]) << 32;
    case 4: k1 ^= ((u64)tail[3]) << 24;
    case 3: k1 ^= ((u64)tail[2]) << 16;
    case 2: k1 ^= ((u64)tail[1]) << 8;
    case 1: k1 ^= ((u64)tail[0]) << 0;
        k1 *= c1;
        k1 = (k1 << 31) | (k1 >> 33);
        k1 *= c2;
        h1 ^= k1;
    }

    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 ^= h1 >> 33;
    h1 *= 0xff51afd7ed558ccdULL;
    h1 ^= h1 >> 33;
    h1 *= 0xc4ceb9fe1a85ec53ULL;
    h1 ^= h1 >> 33;

    h2 ^= h2 >> 33;
    h2 *= 0xff51afd7ed558ccdULL;
    h2 ^= h2 >> 33;
    h2 *= 0xc4ceb9fe1a85ec53ULL;
    h2 ^= h2 >> 33;

    h1 += h2;
    h2 += h1;

    return h1;
}