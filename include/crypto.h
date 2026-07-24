#ifndef UEX_CRYPTO_H_
#define UEX_CRYPTO_H_

#include <stdint.h>
#include <stddef.h>

#define SHA256_BLOCK_SIZE 64
#define SHA256_HASH_SIZE 32

typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    unsigned long long bitlen;
    uint32_t state[8];
} sha256_ctx;

void sha256_init(sha256_ctx *ctx);
void sha256_update(sha256_ctx *ctx, const uint8_t data[], size_t len);
void sha256_final(sha256_ctx *ctx, uint8_t hash[]);

void hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t out[SHA256_HASH_SIZE]);

#endif // UEX_CRYPTO_H_
