#ifndef FLAT_INCLUDES
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/source.h"
#include "def.h"
#endif

bool glb_toc_load_from_source (glb_toc * toc, convert_source * source);
bool gltf_load_from_source (gltf * gltf, glb_toc * toc, convert_source * source);
