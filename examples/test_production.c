#include <stdio.h>
#include <string.h>
#include "uex.h"

int main() {
    printf("=== UEX v2 (Cloud-Native & Secure) Test ===\n");
    if (!uex_init()) {
        printf("FAILED: UEX runtime could not be initialized.\n");
        return 1;
    }

    printf("[+] Successfully initialized UEX runtime.\n");

    uex_chunk_t mani_chunk;
    if (uex_get_chunk("MANI", &mani_chunk)) {
        printf("[+] Found MANI chunk (size: %llu bytes)\n", (unsigned long long)mani_chunk.length);
        printf("    Signature Validated: ");
        for(int i=0; i<8; i++) printf("%02x", mani_chunk.signature[i]);
        printf("...\n");
        printf("    Content:\n%.*s\n", (int)mani_chunk.length, (const char*)mani_chunk.data);
    } else {
        printf("[-] WARNING: MANI chunk not found or signature invalid.\n");
    }

    uex_chunk_t plug_chunk;
    if (uex_get_chunk("PLUG", &plug_chunk)) {
        printf("[+] Found PLUG chunk (WASM) (size: %llu bytes)\n", (unsigned long long)plug_chunk.length);
        printf("    Signature Validated!\n");
        printf("    Simulating WASM Sandbox Initialization...\n");
    }

    uex_chunk_t aimd_chunk;
    if (uex_get_chunk("AIMD", &aimd_chunk)) {
        if (aimd_chunk.is_link) {
            printf("[+] Found AIMD Lazy-Link!\n");
            printf("    URL: %.*s\n", (int)aimd_chunk.length, (const char*)aimd_chunk.data);
            printf("    Simulating Cloud Download of AI Model...\n");
        } else {
            printf("[+] Found local AIMD chunk.\n");
        }
    }

    return 0;
}
