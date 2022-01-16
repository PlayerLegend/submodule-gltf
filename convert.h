#ifndef FLAT_INCLUDES
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/def.h"
#include "def.h"
#endif

bool glb_toc_load_from_interface (glb_toc * toc, window_unsigned_char * buffer, convert_interface * interface);
bool gltf_load_from_interface (gltf * gltf, glb_toc * toc, window_unsigned_char * buffer, convert_interface * interface);
