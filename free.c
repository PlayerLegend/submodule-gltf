#include "free.h"
#include <stdlib.h>

static void gltf_asset_clear (gltf_asset * target)
{
    free (target->generator);
    free (target->version);
}

static void gltf_buffer_clear (gltf_buffer * target)
{
    free (target->uri);
}

static void gltf_material_clear (gltf_material * target)
{
    free (target->name);
}

static void gltf_mesh_primitive_clear (gltf_mesh_primitive * target)
{
    free (target->attributes.texcoord.begin);
    free (target->attributes.color.begin);
    free (target->attributes.joints.begin);
    free (target->attributes.weights.begin);

    free (target->targets.begin);
}

static void gltf_mesh_clear (gltf_mesh * target)
{
    free (target->name);
    gltf_mesh_primitive * target_primitive;

    for_range (target_primitive, target->primitives)
    {
	gltf_mesh_primitive_clear(target_primitive);
    }

    free (target->primitives.begin);
}

void gltf_clear (gltf * gltf)
{
    gltf_asset_clear (&gltf->asset);

#define gltf_clear_array(type, name)		\
    { type * i; for_range (i, gltf->name) type##_clear(i); free (gltf->name.begin); }

    gltf_clear_array (gltf_buffer, buffers);
    free (gltf->buffer_views.begin);
    free (gltf->accessors.begin);
    gltf_clear_array (gltf_material, materials);
    gltf_clear_array (gltf_mesh, meshes);
}

void glb_clear(glb * glb)
{
    gltf_clear(&glb->gltf);
    free (glb->toc_mem);
}
