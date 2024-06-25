#ifndef TG_SHA256_H
#define TG_SHA256_H

#ifdef __cplusplus
extern "C" {
#endif

	
    #include <stddef.h>
    #include <stdint.h>

    typedef struct {
        uint8_t data[64];
        uint32_t datalen;
        uint64_t bitlen;
        uint32_t state[8];
    } sha256_t;

    void sha256_init(sha256_t* sha);
    void sha256_update(sha256_t* sha, const uint8_t data[], size_t len);
    void sha256_final(sha256_t* sha, uint8_t hash[]);
    void sha256_compute(const char* str, char output_buffer[65]);

#ifdef __cplusplus
}
#endif

#endif // !TG_SHA256_H
