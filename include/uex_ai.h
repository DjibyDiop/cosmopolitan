#ifndef UEX_AI_H_
#define UEX_AI_H_

#include <stddef.h>
#include <stdint.h>
#include "uex_loader.h"

/* 
 * UEX AI Model Integration API
 * Facilitates zero-copy loading of models (like GGUF) directly from 
 * the in-memory VFS, minimizing RAM overhead and maximizing startup speed.
 */

typedef void* uex_ai_model_handle_t;

/* 
 * Map a model directly from the VFS chunk into addressable memory 
 * without unpacking it to disk. 
 */
uex_ai_model_handle_t uex_ai_map_model(uex_vfs_handle_t vfs, const char *model_path);

/* Get the direct memory pointer to the loaded model data */
const void* uex_ai_get_model_data(uex_ai_model_handle_t handle);

/* Get the size of the loaded model */
size_t uex_ai_get_model_size(uex_ai_model_handle_t handle);

/* Release the memory mapping */
void uex_ai_unmap_model(uex_ai_model_handle_t handle);

#endif /* UEX_AI_H_ */
