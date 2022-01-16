#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../window/alloc.h"
#include "../keyargs/keyargs.h"
#include "../json/def.h"
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
    convert_interface_fd fd_read = convert_interface_fd_init(fd);

    window_unsigned_char buffer = {0};
    
    if (!glb_toc_load_from_interface(&toc, &buffer, &fd_read.interface))
    {
	log_fatal ("Failed to load from interface");
    }

    convert_clear(&fd_read.interface);
    
    printf ("%.*s\n", toc.json->length, (const char*)toc.json->data);

    return 0;

fail:
    return 1;
}
