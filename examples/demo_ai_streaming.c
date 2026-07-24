#include <stdio.h>
#include <stdlib.h>
#include "../include/uex.h"
#include "../include/uex_loader.h"
#include "../include/uex_ai.h"
#include "../include/uex_plugin.h"

int main() {
    printf("--- UEX Phase 3: Intelligent Executable Demo ---\n\n");
    
    // Simulate initializing the VFS from the UEX payload chunk
    uex_vfs_handle_t vfs = uex_vfs_init((const void *)0x1000, 2048);
    if (!vfs) {
        printf("Failed to initialize VFS.\n");
        return 1;
    }
    
    // 1. Zero-Copy AI Model Streaming
    printf("[*] Testing AI Model Integration\n");
    uex_ai_model_handle_t ai_model = uex_ai_map_model(vfs, "/models/llama-3-8b.gguf");
    if (ai_model) {
        size_t size = uex_ai_get_model_size(ai_model);
        printf("    -> Model loaded directly from UEX container!\n");
        printf("    -> Data mapped at: %p (Size: %lu bytes)\n", uex_ai_get_model_data(ai_model), (unsigned long)size);
        uex_ai_unmap_model(ai_model);
    }
    
    printf("\n");
    
    // 2. Hot-Plugin Loading
    printf("[*] Testing Plugin System\n");
    int plugins_found = uex_plugin_discover();
    if (plugins_found > 0) {
        uex_plugin_handle_t plugin = uex_plugin_load("quantum_compute_extension");
        
        if (plugin) {
            struct uex_plugin_metadata meta;
            if (uex_plugin_get_metadata(plugin, &meta) == 0) {
                printf("    -> Plugin Name: %s\n", meta.name);
                printf("    -> Version: %s\n", meta.version);
            }
            uex_plugin_unload(plugin);
        }
    }
    
    printf("\n[+] Demo completed successfully.\n");
    return 0;
}
