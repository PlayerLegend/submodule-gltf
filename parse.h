#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "def.h"
#endif

bool glb_toc_parse (glb_toc * toc, const range_const_unsigned_char * memory);
bool gltf_parse (gltf * gltf, const range_const_unsigned_char * memory);
bool glb_parse (glb * glb, const range_const_unsigned_char * memory);
void gltf_clear (gltf * gltf);

inline static void glb_clear(glb * glb)
{
    gltf_clear(&glb->gltf);
}
