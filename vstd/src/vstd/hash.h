#ifndef VSTD_HASH_H
#define VSTD_HASH_H
#include <inttypes.h>

// fast but unsafe hashing! collision is common
uint32_t fnv1a_hash(const char* text);

// key can be used as a const char*, len is the length of the key, and seed is random gen, ignore if safety isn't important
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed);

#endif // !VSTD_HASH_H
