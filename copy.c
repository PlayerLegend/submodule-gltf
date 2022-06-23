#include "copy.h"
#include "../log/log.h"
#include <string.h>
#include <stdlib.h>

void * glb_toc_copy_mem(glb_toc * target)
{
    size_t size = target->bin->length + (target->bin->data - (const unsigned char*)target->header);

    void * contents_copy = calloc(1, size);

    memcpy (contents_copy, target->header, size);

    log_debug("toc size %zu", size);

    glb_toc toc_copy = {
	.header = contents_copy,
	.json = (void*)( (char*)contents_copy + ((char*)target->json - (char*)target->header) ),
	.bin = (void*)( (char*)contents_copy + ((char*)target->bin - (char*)target->header) ),
    };

    *target = toc_copy;

    return contents_copy;
}
