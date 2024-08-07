#include "std.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

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


#define ROTL(x,n) (x << n) | (x >> (32 - n))

typedef struct {
    u64 size;
    u32 buffer[4];
    u8 input[64];
    u8 digest[16];
} md5_ctx_t;

#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476
#define F(X, Y, Z) ((X & Y) | (~X & Z))
#define G(X, Y, Z) ((X & Z) | (Y & ~Z))
#define H(X, Y, Z) (X ^ Y ^ Z)
#define I(X, Y, Z) (Y ^ (X | ~Z))

static u32 md5_s[] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

static u32 md5_k[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static u8 PADDING[] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static inline void md5_ini(md5_ctx_t* ctx) {
    ctx->size = (u64)0;
    ctx->buffer[0] = (u32)A;
    ctx->buffer[1] = (u32)B;
    ctx->buffer[2] = (u32)C;
    ctx->buffer[3] = (u32)D;
}


static inline void md5_step(u32* buffer, u32* input) {
    u32 AA = buffer[0];
    u32 BB = buffer[1];
    u32 CC = buffer[2];
    u32 DD = buffer[3];

    u32 E;

    unsigned int j;

    for (unsigned int i = 0; i < 64; ++i) {
        switch (i / 16) {
        case 0:
            E = F(BB, CC, DD);
            j = i;
            break;
        case 1:
            E = G(BB, CC, DD);
            j = ((i * 5) + 1) % 16;
            break;
        case 2:
            E = H(BB, CC, DD);
            j = ((i * 3) + 5) % 16;
            break;
        default:
            E = I(BB, CC, DD);
            j = (i * 7) % 16;
            break;
        }

        u32 temp = DD;
        DD = CC;
        CC = BB;
        BB = BB + ROTL((AA + E + md5_k[i] + input[j]), md5_s[i]);
        AA = temp;
    }

    buffer[0] += AA;
    buffer[1] += BB;
    buffer[2] += CC;
    buffer[3] += DD;
}

static inline void md5_upd(md5_ctx_t* ctx, u8* input_buffer, size_t input_len) {
    u32 input[16];
    unsigned int offset = ctx->size % 64;
    ctx->size += (u64)input_len;

    for (unsigned int i = 0; i < input_len; ++i) {
        ctx->input[offset++] = (u8) * (input_buffer + i);
        if (offset % 64 == 0) {
            for (unsigned int j = 0; j < 16; ++j) {
                input[j] = (u32)(ctx->input[(j * 4) + 3]) << 24 |
                    (u32)(ctx->input[(j * 4) + 2]) << 16 |
                    (u32)(ctx->input[(j * 4) + 1]) << 8 |
                    (u32)(ctx->input[(j * 4)]);
            }
            md5_step(ctx->buffer, input);
            offset = 0;
        }
    }
}

static inline void md5_build(md5_ctx_t* ctx) {
    u32 input[16];
    unsigned int offset = ctx->size % 64;
    unsigned int padding_length = offset < 56 ? 56 - offset : (56 + 64) - offset;

    md5_upd(ctx, PADDING, padding_length);
    ctx->size -= (u64)padding_length;

    for (unsigned int j = 0; j < 14; ++j) {
        input[j] = (u32)(ctx->input[(j * 4) + 3]) << 24 |
            (u32)(ctx->input[(j * 4) + 2]) << 16 |
            (u32)(ctx->input[(j * 4) + 1]) << 8 |
            (u32)(ctx->input[(j * 4)]);
    }
    input[14] = (u32)(ctx->size * 8);
    input[15] = (u32)((ctx->size * 8) >> 32);

    md5_step(ctx->buffer, input);

    for (unsigned int i = 0; i < 4; ++i) {
        ctx->digest[(i * 4) + 0] = (u8)((ctx->buffer[i] & 0x000000FF));
        ctx->digest[(i * 4) + 1] = (u8)((ctx->buffer[i] & 0x0000FF00) >> 8);
        ctx->digest[(i * 4) + 2] = (u8)((ctx->buffer[i] & 0x00FF0000) >> 16);
        ctx->digest[(i * 4) + 3] = (u8)((ctx->buffer[i] & 0xFF000000) >> 24);
    }
}

void md5_to_str(md5_t hash, md5str_t output) {
    for (int i = 0; i < 16; ++i) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[32] = '\0';
}

void md5_str(char* input, md5_t result) {
    md5_ctx_t ctx;
    md5_ini(&ctx);
    md5_upd(&ctx, (u8*)input, strlen(input));
    md5_build(&ctx);

    memcpy(result, ctx.digest, 16);
}

void md5_file(FILE* file, md5_t result) {
    char* input_buffer = (char*)malloc(1024);
    size_t input_size = 0;

    md5_ctx_t ctx;
    md5_ini(&ctx);

    while ((input_size = fread(input_buffer, 1, 1024, file)) > 0) {
        md5_upd(&ctx, (u8*)input_buffer, input_size);
    }

    md5_build(&ctx);

    free(input_buffer);

    memcpy(result, ctx.digest, 16);
}