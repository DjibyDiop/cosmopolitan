#include <stdio.h>
#include <stdlib.h>
#include "../include/uex.h"
#include "../include/uex_manifest.h"

int main() {
    printf("--- UEX Manifest Parsing Demo ---\n");
    
    struct uex_manifest mani;
    
    // In a real scenario, chunk_data comes from reading the UEX binary
    const char *mock_chunk_data = "{ \"version\": 1, \"app_name\": \"test\" }";
    
    if (uex_parse_manifest(mock_chunk_data, &mani) == 0) {
        printf("App Name: %s\n", mani.app_name);
        printf("Min Memory: %u MB\n", mani.min_memory_mb);
        printf("AI Inference Allowed: %s\n", mani.perms.ai_inference ? "Yes" : "No");
        printf("Supported CPUs Mask: 0x%X\n", mani.supported_cpus);
    } else {
        printf("Failed to parse manifest.\n");
    }

    return 0;
}
