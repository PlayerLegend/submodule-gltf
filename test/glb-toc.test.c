#include "../type.h"
#include "../../log/log.h"
#include <fcntl.h>
#include "../../convert/type.h"
#include "../../convert/source.h"
#include "../../convert/fd/source.h"
#include "../load/source.h"
#include <stdio.h>

int main (int argc, char * argv[])
{
    if (argc != 2)
    {
	log_error ("usage: %s [glb-file]", argv[0]);
	return 1;
    }

    glb_toc toc;
    int fd = open (argv[1], O_RDONLY);

    if (fd < 0)
    {
	perror (argv[1]);
	log_fatal ("Could not open input file");
    }

    window_unsigned_char buffer = {0};

    fd_source fd_source = fd_source_init (fd, &buffer);
    
    if (!glb_toc_load_source (&toc, &fd_source.source))
    {
    	log_fatal ("Failed to load from interface");
    }

    convert_source_clear (&fd_source.source);
    
    log_normal ("glb version: %u", toc.header->version);
    log_normal ("file size: %u", toc.header->length);
    log_normal ("json size: %u", toc.json->length);
    log_normal ("bin size: %u", toc.bin->length);

    log_normal ("json data: %.*s", toc.json->length, (const char*)toc.json->data); 

    return 0;

fail:
    return 0;
}
