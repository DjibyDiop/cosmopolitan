#ifndef UEX_MANIFEST_H_
#define UEX_MANIFEST_H_

#include <stdbool.h>
#include <stdint.h>

struct uex_permissions {
    bool network;
    bool gpu;
    bool ai_inference;
    int filesystem_access; /* 0: none, 1: read-only, 2: read-write */
};

struct uex_manifest {
    uint32_t version;
    char app_name[64];
    uint32_t min_memory_mb;
    uint32_t rec_memory_mb;
    uint32_t supported_cpus; /* Bitmask: 1=x86, 2=arm, 4=riscv, 8=wasm */
    struct uex_permissions perms;
};

/* Parses the JSON/binary manifest from a MANI chunk */
int uex_parse_manifest(const void *chunk_data, struct uex_manifest *out_manifest);

#endif /* UEX_MANIFEST_H_ */
