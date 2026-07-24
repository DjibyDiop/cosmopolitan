#ifndef UEX_H_
#define UEX_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * UEX Chunk Representation (v2)
 */
typedef struct {
    char type[5];          // 4-char type + null terminator
    uint64_t length;       // Raw data length (excluding HMAC signature)
    const void* data;      // Zero-Copy Mapped data from Cosmopolitan VFS
    uint8_t signature[32]; // Validated HMAC-SHA256 signature
    bool is_link;          // True if this is a LINK chunk (lazy loading)
} uex_chunk_t;

/**
 * Initializes the UEX v2 runtime.
 * Returns true if the environment is valid.
 */
bool uex_init(void);

/**
 * Retrieves a UEX chunk by its 4-character type.
 * Validates the HMAC-SHA256 signature dynamically.
 * e.g., uex_get_chunk("MANI", &my_chunk);
 * Returns true if found and cryptographic signature matches.
 */
bool uex_get_chunk(const char type[4], uex_chunk_t* out_chunk);

/**
 * Helper to fetch a lazy-loaded chunk (LINK).
 * If the chunk is a LINK, it returns the URL. The application can then fetch it.
 */
const char* uex_get_link_url(const uex_chunk_t* chunk);

#endif // UEX_H_
