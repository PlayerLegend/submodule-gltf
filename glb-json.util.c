#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../window/alloc.h"
#include "../keyargs/keyargs.h"
#include "../json/def.h"
#include "def.h"
#include "../log/log.h"
#include "../convert/source.h"
#include "../convert/fd/source.h"
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

    window_unsigned_char buffer = {0};
    fd_source fd_source = fd_source_init (fd, &buffer);
    
    if (!glb_toc_load_from_source(&toc, &fd_source.source))
    {
	log_fatal ("Failed to load from interface");
    }

    convert_source_clear(&fd_source.source);
    
    printf ("%.*s\n", toc.json->length, (const char*)toc.json->data);

    return 0;

fail:
    return 1;
}
