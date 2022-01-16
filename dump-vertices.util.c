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
#include "../json/traverse.h"
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

    gltf gltf;
    window_unsigned_char buffer = {0};
    
    if (!gltf_load_from_interface(&gltf, &toc, &buffer, &fd_read.interface))
    {
	log_fatal ("Failed to load from interface");
    }

    convert_clear(&fd_read.interface);

    gltf_mesh * mesh;
    gltf_mesh_primitive * mesh_primitive;

    for_range (mesh, gltf.meshes)
    {
	log_normal ("Mesh: %s", mesh->name);
	for_range(mesh_primitive, mesh->primitives)
	{
	    log_normal ("\tPrimitive %d:", range_index(mesh,gltf.meshes));

	    gltf_accessor * position_accessor = mesh_primitive->attributes.position;

	    assert (position_accessor->type == GLTF_ACCESSOR_VEC3);
	    assert (position_accessor->component_type == GLTF_ACCESSOR_COMPONENT_FLOAT);
	    assert (position_accessor->sparse.present == false);

	    range_const_unsigned_char file_bin =
	    {
		.begin = toc.bin->data,
		.end = file_bin.begin + toc.bin->length,
	    };
	    
	    range_const_unsigned_char buffer_bytes =
	    {
		.begin = file_bin.begin,
		.end = buffer_bytes.begin + position_accessor->buffer_view->buffer->byte_length,
	    };
	    
	    range_const_unsigned_char buffer_view_bytes =
	    {
		.begin = buffer_bytes.begin + position_accessor->buffer_view->byte_offset,
		.end = buffer_view_bytes.begin + position_accessor->buffer_view->byte_length,
	    };

	    range_const_unsigned_char accessor_bytes =
	    {
		.begin = buffer_view_bytes.begin + position_accessor->byte_offset,
		.end = accessor_bytes.begin + position_accessor->count * 3 * sizeof(float),
	    };

	    assert (buffer_bytes.begin >= file_bin.begin);
	    assert (buffer_view_bytes.begin >= buffer_bytes.begin);
	    assert (accessor_bytes.begin >= buffer_view_bytes.begin);

	    assert (buffer_bytes.end <= file_bin.end);
	    assert (buffer_view_bytes.end <= buffer_bytes.end);
	    assert (accessor_bytes.end <= buffer_view_bytes.end);

	    float * coordinate_array = (float*) accessor_bytes.begin;

	    for (size_t i = 0; i < position_accessor->count; i++)
	    {
		log_normal ("\t\tPOSITION(%zu): [%f, %f, %f]",
			    i,
			    coordinate_array[3 * i],
			    coordinate_array[3 * i + 1],
			    coordinate_array[3 * i + 2]);
	    }
	}
    }
    
    return 0;

fail:
    return 1;
}
