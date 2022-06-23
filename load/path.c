#include "source.h"
#include "path.h"
#include "../../convert/fd/source.h"
#include <fcntl.h>
#include <stdio.h>
#include "../../convert/source.h"
#include <assert.h>
#include "../../window/alloc.h"
#include "../copy.h"

bool glb_load_path (glb * glb, const char * path)
{
    window_unsigned_char contents = {0};

    fd_source fd_source = fd_source_init(open(path, O_RDONLY), &contents);

    if (fd_source.fd < 0)
    {
	perror(path);
	return false;
    }

    bool retval = glb_load_source(glb, &fd_source.source);

    if (retval)
    {
	glb_copy_mem(glb);
    }

    convert_source_clear(&fd_source.source);

    window_clear(contents);

    return retval;
}
