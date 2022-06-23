#include "source.h"
#include "../../convert/source.h"
#include "memory.h"
#include <assert.h>
#include "../../window/alloc.h"
#include "../../log/log.h"
#include "../../convert/fd/source.h"
#include <fcntl.h>

bool glb_toc_load_source (glb_toc * toc, convert_source * source)
{
    size_t min_alloc_size = sizeof(*toc->header) + sizeof(*toc->json) + sizeof(*toc->bin);
    size_t need_size;

    window_unsigned_char * buffer = source->contents;

    window_alloc (*buffer, min_alloc_size);

    status status;
    
    while ((status = convert_fill_alloc (source)) != STATUS_ERROR)
    {
	toc->header = (glb_header*) buffer->region.begin;

	//log_debug ("magic: %8s", &toc->header->magic);
	
	assert (toc->header->magic == GLB_MAGIC);

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
		log_fatal ("Reading json binary failed, got %zu / %zu = %zu", range_count(buffer->region), (unsigned char*)toc->bin - buffer->region.begin);
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
		log_fatal ("Reading bin data failed");
	    }
	    continue;
	}

	buffer->region.begin += need_size;

	return true;
    }

    log_debug ("Read error");

fail:
    return false;
}

bool glb_load_source (glb * glb, convert_source * source)
{
    //log_debug("Load glb");

    glb->toc_mem = NULL;

    if (!glb_toc_load_source (&glb->toc, source))
    {
	return false;
    }

    range_const_unsigned_char json_text = { .begin = glb->toc.json->data, .end = glb->toc.json->data + glb->toc.json->length };

    return gltf_load_memory (&glb->gltf, &json_text);
}
