#include "uex.h"
#include "crypto.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t UEX_SECRET_KEY[] = "SuperSecretUEXKey2026";
static bool g_initialized = false;

bool uex_init(void) {
    if (g_initialized) return true;
    
    // In Cosmopolitan, the ZIP VFS is automatically initialized by the libc 
    // at startup if a ZIP is attached to the binary.
    // We can just verify if the .uex directory exists in the virtual filesystem.
    struct stat st;
    if (stat("/zip/.uex", &st) == 0 && S_ISDIR(st.st_mode)) {
        g_initialized = true;
        return true;
    }
    
    // If not a directory, maybe there are just files. Let's assume initialized.
    g_initialized = true;
    return true;
}

bool uex_get_chunk(const char type[4], uex_chunk_t* out_chunk) {
    if (!uex_init()) return false;
    if (!out_chunk) return false;

    char filepath[256];
    bool is_link = false;

    // Try .bin first (Direct Data)
    snprintf(filepath, sizeof(filepath), "/zip/.uex/%.4s.bin", type);
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        // Try .lnk (Lazy Link)
        snprintf(filepath, sizeof(filepath), "/zip/.uex/%.4s.lnk", type);
        fd = open(filepath, O_RDONLY);
        if (fd < 0) {
            return false;
        }
        is_link = true;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return false;
    }

    size_t file_size = st.st_size;
    if (file_size < 32) {
        // Too small to contain the HMAC signature
        close(fd);
        return false;
    }

    // Mmap via Cosmopolitan VFS (Zero-Copy directly from the executable on disk!)
    const uint8_t* map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (map == MAP_FAILED) {
        return false;
    }

    // Verify HMAC-SHA256 Signature (Zero-Trust)
    const uint8_t* expected_signature = map;
    const uint8_t* payload = map + 32;
    size_t payload_len = file_size - 32;

    uint8_t computed_signature[32];
    hmac_sha256(UEX_SECRET_KEY, strlen((const char*)UEX_SECRET_KEY), payload, payload_len, computed_signature);

    if (memcmp(expected_signature, computed_signature, 32) != 0) {
        // SECURITY BREACH: Signature mismatch!
        printf("[UEX SECURITY] FATAL: Cryptographic signature mismatch for chunk %.4s!\n", type);
        munmap((void*)map, file_size);
        return false;
    }

    // Populate chunk
    strncpy(out_chunk->type, type, 4);
    out_chunk->type[4] = '\0';
    out_chunk->length = payload_len;
    out_chunk->data = payload;
    memcpy(out_chunk->signature, expected_signature, 32);
    out_chunk->is_link = is_link;

    return true;
}

const char* uex_get_link_url(const uex_chunk_t* chunk) {
    if (!chunk || !chunk->is_link) return NULL;
    return (const char*)chunk->data; // This is a null-terminated string or close to it
}
