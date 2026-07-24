#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uex_plugin.h"

/*
 * UEX Plugin Management
 * Mock implementation of discovery and loading of dynamic plugins
 * embedded within the UEX container's payload chunks.
 */

struct uex_plugin_internal {
    struct uex_plugin_metadata meta;
    void *memory_handle;
};

int uex_plugin_discover(void) {
    printf("[UEX Plugins] Scanning internal container for 'PLUG' chunks...\n");
    // Mock logic: assume we found 2 plugins
    printf("[UEX Plugins] Discovered 2 available plugins.\n");
    return 2;
}

uex_plugin_handle_t uex_plugin_load(const char *plugin_name) {
    if (!plugin_name) return NULL;
    
    printf("[UEX Plugins] Loading plugin: %s\n", plugin_name);
    
    struct uex_plugin_internal *plugin = (struct uex_plugin_internal *)malloc(sizeof(struct uex_plugin_internal));
    if (!plugin) return NULL;
    
    strncpy(plugin->meta.name, plugin_name, 63);
    strncpy(plugin->meta.version, "1.0.0", 15);
    plugin->meta.required_api_version = 1;
    plugin->meta.is_loaded = true;
    
    // Simulate loading the plugin code into memory via uex_dlopen_mem
    plugin->memory_handle = (void*)0x88888888;
    
    printf("[UEX Plugins] Plugin %s successfully initialized and loaded.\n", plugin_name);
    return (uex_plugin_handle_t)plugin;
}

int uex_plugin_get_metadata(uex_plugin_handle_t handle, struct uex_plugin_metadata *out_meta) {
    if (!handle || !out_meta) return -1;
    struct uex_plugin_internal *plugin = (struct uex_plugin_internal *)handle;
    
    memcpy(out_meta, &plugin->meta, sizeof(struct uex_plugin_metadata));
    return 0;
}

void uex_plugin_unload(uex_plugin_handle_t handle) {
    if (!handle) return;
    struct uex_plugin_internal *plugin = (struct uex_plugin_internal *)handle;
    
    printf("[UEX Plugins] Unloading plugin: %s\n", plugin->meta.name);
    plugin->meta.is_loaded = false;
    free(plugin);
}
