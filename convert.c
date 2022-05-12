#include "convert.h"
#include "parse.h"
#include <assert.h>
#include "../window/alloc.h"
#include "../log/log.h"
#include "../convert/fd/source.h"
#include <fcntl.h>

bool glb_toc_load_from_source (glb_toc * toc, convert_source * source)
{
    size_t min_alloc_size = sizeof(*toc->header) + sizeof(*toc->json) + sizeof(*toc->bin);
    size_t need_size;

    window_unsigned_char * buffer = source->contents;

    window_alloc (*buffer, min_alloc_size);

    status status;
    
    while ((status = convert_fill_alloc (source)) != STATUS_ERROR)
    {
	toc->header = (glb_header*) buffer->region.begin;

	assert ((size_t)range_count (buffer->region) >= min_alloc_size);
	toc->json = (glb_chunk_header*) (buffer->region.begin + sizeof(*toc->header));
	assert ((unsigned char*) (toc->json + 1) < buffer->region.end);

	toc->bin = (glb_chunk_header*) ((unsigned char*)(toc->json + 1) + toc->json->length);
	
	if ((unsigned char*)(toc->bin + 1) > buffer->region.end)
	{
	    if (status == STATUS_UPDATE)
	    {
		need_size = min_alloc_size + toc->json->length;
		window_alloc (*buffer, need_size);
		continue;
	    }
	    else
	    {
		break;
	    }
	}

	if ((unsigned char*)(toc->bin + 1) + toc->bin->length > buffer->region.end)
	{
	    if (status == STATUS_UPDATE)
	    {
		need_size = min_alloc_size + toc->json->length + toc->bin->length;
		window_alloc (*buffer, need_size);
	    }
	    else
	    {
		break;
	    }
	    continue;
	}

	buffer->region.begin += need_size;

	return true;
    }

    return false;
}

bool glb_load_from_source (glb * glb, convert_source * source)
{
    if (!glb_toc_load_from_source (&glb->toc, source))
    {
	return false;
    }

    range_const_unsigned_char json_text = { .begin = glb->toc.json->data, .end = glb->toc.json->data + glb->toc.json->length };

    return gltf_parse (&glb->gltf, &json_text);
}
