#ifndef UEX_LOADER_H_
#define UEX_LOADER_H_

#include <stddef.h>

/* Virtual File System Handle */
typedef void* uex_vfs_handle_t;

/* In-Memory Dynamic Library Handle */
typedef void* uex_dynlib_handle_t;

/* Initialize the VFS from a VFS_ chunk */
uex_vfs_handle_t uex_vfs_init(const void *chunk_data, size_t size);

/* Load a shared library (.so, .dll, .dylib) directly from memory/VFS */
uex_dynlib_handle_t uex_dlopen_mem(uex_vfs_handle_t vfs, const char *lib_path);

/* Get a symbol address from a loaded memory library */
void *uex_dlsym(uex_dynlib_handle_t handle, const char *symbol);

/* Close and free memory library */
int uex_dlclose(uex_dynlib_handle_t handle);

#endif /* UEX_LOADER_H_ */
