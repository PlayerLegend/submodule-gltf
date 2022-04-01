#include "../def.h"
#include "../../log/log.h"
#include <fcntl.h>
#include "../../convert/status.h"
#include "../../convert/source.h"
#include "../../convert/fd/source.h"
#include "../convert.h"
#include <assert.h>

int main (int argc, char * argv[])
{
    if (argc != 2)
    {
	log_error ("usage: %s [glb-file]", argv[0]);
	return 1;
    }

    glb glb;
    int fd = open (argv[1], O_RDONLY);
    window_unsigned_char buffer = {0};    
    fd_source fd_source = fd_source_init (fd, &buffer);

    if (!glb_load_from_source(&glb, &fd_source.source))
    {
	log_fatal ("Failed to load from source");
    }

    convert_source_clear(&fd_source.source);

    gltf_mesh * mesh;
    gltf_mesh_primitive * mesh_primitive;

    for_range (mesh, glb.gltf.meshes)
    {
	log_normal ("Mesh: %s", mesh->name);
	for_range(mesh_primitive, mesh->primitives)
	{
	    log_normal ("\tPrimitive %d:", range_index(mesh, glb.gltf.meshes));

	    gltf_accessor * position_accessor = mesh_primitive->attributes.position;

	    assert (position_accessor->type == GLTF_ACCESSOR_VEC3);
	    assert (position_accessor->component_type == GLTF_ACCESSOR_COMPONENT_FLOAT);
	    assert (position_accessor->sparse.present == false);

	    range_const_unsigned_char file_bin =
	    {
		.begin = glb.toc.bin->data,
		.end = file_bin.begin + glb.toc.bin->length,
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
