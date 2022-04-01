#include "def.h"
#include <stdlib.h>
#include "../range/string.h"
#include "../table/string.h"
#include "../json/def.h"
#include "../json/parse.h"
#include "../keyargs/keyargs.h"
#include "../json/traverse.h"
#include "../log/log.h"
#include <assert.h>
#include <string.h>

// misc helpers

//#define fatal(...) { log_error(__VA_ARGS__); return false; }
#define bound_index(index, range) (index <= (size_t)range_count(range))

static void * _reference_array(range_void * array, size_t item_size, const json_object * parent, const char * name)
{
    bool success = true;
    size_t count = ((char*)array->end - (char*)array->begin) / item_size;
    size_t index = json_get_number(.success = &success, .parent = parent, .key = name);

    if (index >= count)
    {
	log_fatal ("Index out of range for %s, (index)%zd >= (count)%zd", name, index, count);
    }

    return (char*)array->begin + index * item_size;

fail:
    return NULL;
}

#define reference_array(array, parent, name) _reference_array((range_void*)&(array), sizeof(*(array).begin), parent, name)

#define read_array(...) keyargs_call(read_array, __VA_ARGS__)
keyargs_declare_static(bool, read_array,
		       range_void * output;
		       const json_array * input;
		       gltf * gltf;
		       size_t item_size;
		       bool optional;
		       bool (*function)(void * output, gltf * gltf, const json_object * input););

keyargs_define_static(read_array)		       
{
    if (!args.input)
    {
	return args.optional;
    }

    assert (args.item_size);
    assert (args.output);
    assert (args.gltf);
    
    size_t count = range_count (*args.input);

    args.output->begin = calloc (count, args.item_size);
    args.output->end = (char*)args.output->begin + (count * args.item_size);

    json_value * value;
    
    for (size_t i = 0; i < count; i++)
    {
	value = args.input->begin + i;

	if (value->type != JSON_OBJECT)
	{
	    log_fatal ("JSON Array member is not an object");
	}

	if (!args.function ((char*)args.output->begin + (i * args.item_size), args.gltf, value->object))
	{
	    log_fatal ("Failed to read array element");
	}
    }

    return true;

fail:
    return false;
}

static char * _dupe_string (const char * input)
{
    return input ? strcpy(malloc(strlen(input) + 1), input) : NULL;
}

static bool _set_accessor_type (gltf_accessor_type * type_int, const char * type_string)
{
    struct { const char * key; gltf_accessor_type value; }
    map[] =
	{
	    { "SCALAR", GLTF_ACCESSOR_SCALAR },
	    { "VEC2", GLTF_ACCESSOR_VEC2 },
	    { "VEC3", GLTF_ACCESSOR_VEC3 },
	    { "VEC4", GLTF_ACCESSOR_VEC4 },
	    { "MAT2", GLTF_ACCESSOR_MAT2 },
	    { "MAT3", GLTF_ACCESSOR_MAT3 },
	    { "MAT4", GLTF_ACCESSOR_MAT4 },
	};

    for (size_t i = 0; i < sizeof (map) / sizeof (*map); i++)
    {
	if (0 == strcmp (type_string, map[i].key))
	{
	    *type_int = map[i].value;
	    return true;
	}
    }

    *type_int = GLTF_ACCESSOR_BADTYPE;

    return false;
}

static bool _validate_component_type (size_t type)
{
    switch (type)
    {
    case GLTF_ACCESSOR_COMPONENT_BYTE:
    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_BYTE:
    case GLTF_ACCESSOR_COMPONENT_SHORT:
    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_SHORT:
    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_INT:
    case GLTF_ACCESSOR_COMPONENT_FLOAT:
	return true;

    default:
	return false;
    }

    return false;
}

static size_t _get_accessor_component_size (gltf_accessor_component_type component_type)
{
    switch (component_type)
    {
    case GLTF_ACCESSOR_COMPONENT_BYTE:
	return 1;

    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_BYTE:
	return 1;
	
    case GLTF_ACCESSOR_COMPONENT_SHORT:
	return 2;

    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_SHORT:
	return 2;

    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_INT:
	return 4;
	
    case GLTF_ACCESSOR_COMPONENT_FLOAT:
	return 4;
	
    default:
	return 0;
    }

    return 0;
}

// glb_toc_*

bool glb_toc_load_memory (glb_toc * toc, const range_const_unsigned_char * memory)
{
    if ((size_t) range_count (*memory) < sizeof(*toc->header) + sizeof(*toc->json) + sizeof(*toc->bin))
    {
	return false;
    }

    toc->header = (const glb_header*) memory->begin;
    toc->json = (const glb_chunk_header*)(memory->begin + sizeof(*toc->header));
    toc->bin = (const glb_chunk_header*)((const char*) (toc->json + 1) + toc->json->length);

    if ( (const unsigned char*)(toc->bin + 1) + toc->bin->length > memory->end)
    {
	return false;
    }

    return true;
}

// _get_json_* functions

typedef struct number_arg number_arg;
struct number_arg
{
    const char * name;
    size_t default_value;
    bool optional;
};

typedef struct object_arg object_arg;
struct object_arg
{
    const char * name;
    bool optional;
};

// _gltf_read_* single elements

static bool _gltf_read_asset (gltf_asset * gltf_asset, const json_object * json_asset)
{
    if (!json_asset)
    {
	return false;
    }
    
    bool success = true;
    
    gltf_asset->generator = _dupe_string(json_get_string(.success = &success, .parent = json_asset, .key = "generator"));
    gltf_asset->version	  = _dupe_string(json_get_string(.success = &success, .parent = json_asset, .key = "version"));

    return success;
}

static bool _gltf_read_buffer (void * output, gltf * gltf, const json_object * json_buffer)
{
    gltf_buffer * gltf_buffer = output;
    
    bool success = true;

    gltf_buffer->byte_length = json_get_number(.parent = json_buffer, .key = "byteLength", .success = &success);

    if (!success)
    {
	return false;
    }

    json_pair * uri_pair = json_lookup_string (json_buffer, "uri");

    if (!uri_pair || uri_pair->value.type == JSON_NULL)
    {
	gltf_buffer->uri = NULL;
    }
    else if (uri_pair->value.type == JSON_STRING)
    {
	gltf_buffer->uri = strcpy (malloc (strlen (uri_pair->value.string) + 1), uri_pair->value.string);
    }
    else 
    {
	log_error ("GLTF JSON buffer uri is present but is not a string");
	return false;
    }

    return true;
}

static bool _gltf_read_buffer_view (void * output, gltf * gltf, const json_object * json_buffer_view)
{
    gltf_buffer_view * gltf_buffer_view = output;

    bool success = true;

    gltf_buffer_view->buffer = reference_array(gltf->buffers, json_buffer_view, "buffer");
    gltf_buffer_view->byte_length = json_get_number(.success = &success, .parent = json_buffer_view, .key = "byteLength");
    gltf_buffer_view->byte_offset = json_get_number(.success = &success, .parent = json_buffer_view, .key = "byteOffset", .optional = true);
    gltf_buffer_view->byte_stride = json_get_number(.success = &success, .parent = json_buffer_view, .key = "byteStride", .optional = true, .default_value = 0);
    
    if (!success || !gltf_buffer_view->buffer)
    {
	return false;
    }

    return true;
}

static bool _gltf_read_accessor_sparse (gltf_accessor * gltf_accessor, gltf * gltf, const json_object * json_sparse)
{
    const json_object * sparse_indices;
    const json_object * sparse_values;

    sparse_indices = json_get_object (.parent = json_sparse, .key = "indices");
    sparse_values = json_get_object (.parent = json_sparse, .key = "values");
        
    if (!sparse_indices || !sparse_values)
    {
	log_error ("GLTF JSON failed to read sparse accessor");
	return false;
    }

    bool success = true;

    gltf_accessor->sparse.count			 = json_get_number(.success = &success, .parent = json_sparse, .key = "count");
    gltf_accessor->sparse.indices.byte_offset	 = json_get_number(.success = &success, .parent = sparse_values, .key = "byteOffset");
    gltf_accessor->sparse.indices.component_type = json_get_number(.success = &success, .parent = sparse_indices, .key = "componentType");

    gltf_accessor->sparse.indices.buffer_view = reference_array (gltf->buffer_views, sparse_indices, "bufferView");
    gltf_accessor->sparse.values.buffer_view  = reference_array (gltf->buffer_views, sparse_values, "bufferView");
        
    if (!success || !gltf_accessor->sparse.indices.buffer_view || !gltf_accessor->sparse.values.buffer_view)
    {
	log_error ("GLTF JSON failed to read sparse accessor members");
	return false;	
    }

    if (!_validate_component_type(gltf_accessor->sparse.indices.component_type))
    {
	log_error ("GLTF JSON sparse accessor has invalid indices component type");
	return false;
    }

    size_t component_size = _get_accessor_component_size(gltf_accessor->component_type);

    if ((gltf_accessor->byte_offset + gltf_accessor->buffer_view->byte_offset) % component_size)
    {
	log_error ("GLTF JSON sparse accessor is not aligned properly to size %zd", component_size);
	return false;
    }

    return true;
}

static bool _gltf_read_accessor (void * output, gltf * gltf, const json_object * json_accessor)
{
    gltf_accessor * gltf_accessor = output;
    
    const char * accessor_type;
    bool success;

    const json_object * sparse = json_get_object (.parent = json_accessor, .key = "sparse", .optional = true);
        
    if (sparse)
    {
	gltf_accessor->sparse.present = true;
	if (!_gltf_read_accessor_sparse(gltf_accessor, gltf, sparse))
	{
	    log_fatal ("GLTF JSON failed to read sparse accessor");
	}
    }

    success = true;

    gltf_accessor->byte_offset	  = json_get_number(.success = &success, .parent = json_accessor, .key = "byteOffset", .optional = true, .default_value = 0);
    gltf_accessor->component_type = json_get_number(.success = &success, .parent = json_accessor, .key = "componentType");
    gltf_accessor->count	  = json_get_number(.success = &success, .parent = json_accessor, .key = "count");
    gltf_accessor->normalized     = json_get_bool(.success = &success, .parent = json_accessor, .key = "normalized", .optional = true, .default_value = false);
    accessor_type		  = json_get_string(.success = &success, .parent = json_accessor, .key = "type");

    if (gltf_accessor->normalized == true && (gltf_accessor->component_type == GLTF_ACCESSOR_COMPONENT_FLOAT || gltf_accessor->component_type == GLTF_ACCESSOR_COMPONENT_UNSIGNED_INT))
    {
	log_fatal ("Normalized components specified with component type float or uint32 is invalid");
    }
    
    if (!success)
    {
	log_fatal ("GLTF JSON failed to read component of accessor");
    }

    if (!_validate_component_type (gltf_accessor->component_type))
    {
	log_fatal ("GLTF JSON accessor has invalid component type");
    }
    
    if (!_set_accessor_type (&gltf_accessor->type, accessor_type))
    {
	log_fatal ("GLTF JSON accessor has invalid type");
    }

    gltf_accessor->buffer_view = reference_array (gltf->buffer_views, json_accessor, "bufferView");

    if (!gltf_accessor->buffer_view && !gltf_accessor->sparse.present)
    {
	log_fatal("GLTF JSON accessor has neither a bufferView index nor sparse accessor");
    }
    
    return true;

fail:
    return false;
}

static bool _gltf_read_material (void * output, gltf * gltf, const json_object * json_material)
{
    gltf_material * material = output;

    bool success = true;

    material->name = _dupe_string (json_get_string(.success = &success,
						   .parent = json_material,
						   .key = "name"));

    const json_object * pbr_metallic_roughness = json_get_object(.success = &success,
								 .parent = json_material,
								 .key = "pbrMetallicRoughness");
    
    material->double_sided = json_get_bool(.success = &success,
					   .parent = json_material,
					   .key = "doubleSided");

    material->metallic_factor = json_get_number(.success = &success,
						.parent = pbr_metallic_roughness,
						.key = "metallicFactor");
    
    material->roughness_factor = json_get_number(.success = &success,
						 .parent = pbr_metallic_roughness,
						 .key = "roughnessFactor");

    return success;
}

static bool _gltf_read_mesh_primitive_target (void * output, gltf * gltf, const json_object * json_target)
{
    gltf_mesh_primitive_target * gltf_target = output;
    
    gltf_target->normal	  = reference_array (gltf->accessors, json_target, "NORMAL");
    gltf_target->position = reference_array (gltf->accessors, json_target, "POSITION");
    gltf_target->tangent  = reference_array (gltf->accessors, json_target, "TANGENT");

    return gltf_target->normal && gltf_target->position && gltf_target->tangent;
}

static bool _gltf_read_mesh_primitive (void * output, gltf * gltf, const json_object * json_primitive)
{
    gltf_mesh_primitive * primitive = output;
    
    primitive->indices	= reference_array (gltf->accessors, json_primitive, "indices");
    primitive->material = reference_array (gltf->materials, json_primitive, "material");

    const json_object * mesh_primitive_attributes = json_get_object(.parent = json_primitive,
								    .key = "attributes");

    if (!mesh_primitive_attributes)
    {
	return false;
    }

    primitive->attributes.position = reference_array (gltf->accessors, mesh_primitive_attributes, "POSITION");
    primitive->attributes.normal = reference_array (gltf->accessors, mesh_primitive_attributes, "NORMAL");

    if (!read_array (.output = (range_void*)&primitive->targets,
		     .gltf = gltf,
		     .input = json_get_array (.parent = json_primitive,
					      .key = "targets",
					      .optional = true),
		     .optional = true,
		     .function = _gltf_read_mesh_primitive_target,
		     .item_size = sizeof(*primitive->targets.begin)))
    {
	log_fatal ("Failed to read mesh primitive targets array");
    }
    
    if (!primitive->indices)
    {
	log_fatal ("No indices specified in mesh primitive");
    }

    if (!primitive->material)
    {
	log_fatal ("No materials specified in mesh primitive");
    }

    return true;

fail:
    return false;
}

static bool _gltf_read_mesh (void * output, gltf * gltf, const json_object * json_mesh)
{
    gltf_mesh * gltf_mesh = output;

    bool success = true;
    
    gltf_mesh->name = _dupe_string (json_get_string(.success = &success,
						    .parent = json_mesh,
						    .key = "name"));

    if (!success)
    {
	return false;
    }
    
    if (!read_array (.output = (range_void*)&gltf_mesh->primitives,
		     .gltf = gltf,
		     .input = json_get_array (.parent = json_mesh,
					      .key = "primitives"),
		     .function = _gltf_read_mesh_primitive,
		     .item_size = sizeof(*gltf_mesh->primitives.begin)))
    {
	log_fatal ("Could not read mesh primitives");
    }

    return true;

fail:
    return false;
}

//

static bool gltf_from_json (gltf * gltf, json_value * json_root_value)
{
    memset (gltf, 0, sizeof (*gltf));

    if (json_root_value->type != JSON_OBJECT)
    {
	log_fatal ("GLTF JSON data is not an object");
    }
    
    json_object * json_root_object = json_root_value->object;

    // reading gltf members:
    
    //    asset
    if (!_gltf_read_asset (&gltf->asset,
			   json_get_object(.parent = json_root_object,
					   .key = "asset")))
    {
	log_fatal ("GLTF JSON failed to load asset");
    }

    //    buffers
    if (!read_array (.output = (range_void*)&gltf->buffers,
		     .input = json_get_array (.parent = json_root_object,
					      .key = "buffers"),
		     .function = _gltf_read_buffer,
		     .gltf = gltf,
		     .item_size = sizeof(*gltf->buffers.begin)))
    {
	log_fatal ("GLTF JSON failed to load buffers");
    }

    //    buffer views
    if (!read_array (.output = (range_void*)&gltf->buffer_views,
		     .input = json_get_array (.parent = json_root_object,
					      .key = "bufferViews"),
		     .function = _gltf_read_buffer_view,
		     .gltf = gltf,
		     .item_size = sizeof(*gltf->buffer_views.begin)))
    {
	log_fatal ("GLTF JSON failed to load bufferViews");
    }

    //    accessors
    if (!read_array (.output = (range_void*)&gltf->accessors,
		     .input = json_get_array (.parent = json_root_object,
					      .key = "accessors"),
		     .function = _gltf_read_accessor,
		     .gltf = gltf,
		     .item_size = sizeof(*gltf->accessors.begin)))
    {
	log_fatal ("GLTF JSON failed to load accessors");
    }

    //    materials
    // ...todo
    if (!read_array (.output = (range_void*)&gltf->materials,
		     .input = json_get_array (.parent = json_root_object,
					      .key = "materials"),
		     .function = _gltf_read_material,
		     .gltf = gltf,
		     .item_size = sizeof(*gltf->materials.begin)))
    {
	log_fatal ("GLTF JSON failed to load materials");
    }

    //    meshes
    if (!read_array (.output = (range_void*)&gltf->meshes,
		     .input = json_get_array (.parent = json_root_object,
					      .key = "meshes"),
		     .function = _gltf_read_mesh,
		     .gltf = gltf,
		     .item_size = sizeof(*gltf->meshes.begin)))
    {
	log_fatal ("GLTF JSON failed to load meshes");
    }
    
    return true;
    
fail:
    return false;
}

bool gltf_parse (gltf * gltf, const range_const_unsigned_char * memory)
{
    json_value * json_root = json_parse(&memory->char_cast.const_cast);

    bool status = gltf_from_json(gltf, json_root);

    json_value_free(json_root);

    return status;
}

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
