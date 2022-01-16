#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../keyargs/keyargs.h"
#include "../json/def.h"
#include "../window/def.h"
#include "../window/alloc.h"
#include "../convert/def.h"
#include "def.h"
#include "parse.h"

bool glb_toc_load_from_interface (glb_toc * toc, window_unsigned_char * buffer, convert_interface * interface)
{
    bool error = false;

    size_t min_alloc_size = sizeof(*toc->header) + sizeof(*toc->json) + sizeof(*toc->bin);
    size_t need_size;

    window_alloc (*buffer, min_alloc_size);

    while (convert_fill (&error, buffer, interface))
    {
	toc->header = (glb_header*) buffer->region.begin;
	
	assert ((size_t)range_count (buffer->region) >= min_alloc_size);
	toc->json = (glb_chunk_header*) (buffer->region.begin + sizeof(*toc->header));
	assert ((unsigned char*) (toc->json + 1) < buffer->region.end);

	toc->bin = (glb_chunk_header*) ((unsigned char*)(toc->json + 1) + toc->json->length);

	if ((unsigned char*)(toc->bin + 1) > buffer->region.end)
	{
	    need_size = min_alloc_size + toc->json->length;
	    window_alloc (*buffer, need_size);
	    continue;
	}

	if ((unsigned char*)(toc->bin + 1) + toc->bin->length > buffer->region.end)
	{
	    need_size = min_alloc_size + toc->json->length + toc->bin->length;
	    window_alloc (*buffer, need_size);
	    continue;
	}

	buffer->region.begin += need_size;

	return true;
    }

    return false;
}

bool gltf_load_from_interface (gltf * gltf, glb_toc * toc, window_unsigned_char * buffer, convert_interface * interface)
{
    if (!glb_toc_load_from_interface (toc, buffer, interface))
    {
	return false;
    }

    range_const_unsigned_char json_text = { .begin = toc->json->data, .end = toc->json->data + toc->json->length };

    return gltf_parse (gltf, &json_text);
}
