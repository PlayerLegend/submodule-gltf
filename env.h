#ifndef FLAT_INCLUDES
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../vec/vec.h"
#include "../vec/vec3.h"
#include "type.h"
#endif

bool gltf_accessor_env_setup (gltf_accessor_env * env, const glb_toc * toc, gltf_accessor * import_accessor);
bool gltf_accessor_env_load_fvec3 (void * target, bool (*loader)(void * target, const fvec3 * input), range_gltf_index * indices, gltf_accessor_env * env);
bool gltf_accessor_env_load_indices (window_gltf_index * output, gltf_accessor_env * env);
