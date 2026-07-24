#ifndef UEX_PLUGIN_H_
#define UEX_PLUGIN_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * UEX Hot-Plugin Loading API
 * Allows UEX binaries to discover, validate, and load dynamic modules (plugins)
 * at runtime without altering the root executable signature.
 */

typedef void* uex_plugin_handle_t;

struct uex_plugin_metadata {
    char name[64];
    char version[16];
    uint32_t required_api_version;
    bool is_loaded;
};

/* 
 * Discover available plugins within the UEX container's VFS 
 */
int uex_plugin_discover(void);

/* 
 * Load a specific plugin by name and execute its initialization callback
 */
uex_plugin_handle_t uex_plugin_load(const char *plugin_name);

/* 
 * Query metadata for a loaded plugin
 */
int uex_plugin_get_metadata(uex_plugin_handle_t handle, struct uex_plugin_metadata *out_meta);

/* 
 * Unload a plugin gracefully 
 */
void uex_plugin_unload(uex_plugin_handle_t handle);

#endif /* UEX_PLUGIN_H_ */
