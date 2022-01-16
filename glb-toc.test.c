#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../keyargs/keyargs.h"
#include "../json/def.h"
#include "../window/def.h"
#include "def.h"
#include "../log/log.h"
#include "../convert/def.h"
#include "../convert/fd.h"
#include "convert.h"

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
    
    convert_interface_fd fd_read = convert_interface_fd_init(fd);

    window_unsigned_char buffer = {0};

    if (!glb_toc_load_from_interface (&toc, &buffer, &fd_read.interface))
    {
    	log_fatal ("Failed to load from interface");
    }

    convert_clear (&fd_read.interface);
    
    log_normal ("glb version: %u", toc.header->version);
    log_normal ("file size: %u", toc.header->length);
    log_normal ("json size: %u", toc.json->length);
    log_normal ("bin size: %u", toc.bin->length);

    log_normal ("json data: %.*s", toc.json->length, (const char*)toc.json->data); 

    return 0;

fail:
    return 0;
}
