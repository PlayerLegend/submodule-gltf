#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../window/alloc.h"
#include "../vec/vec.h"
#include "../vec/vec3.h"
#include "type.h"
#include "env.h"
#include "../log/log.h"

bool gltf_accessor_env_setup (gltf_accessor_env * env, const glb_toc * toc, gltf_accessor * import_accessor)
{
    env->accessor = import_accessor;

    env->type = env->accessor->type;
    env->normalized = env->accessor->normalized;

    gltf_buffer_view * buffer_view = env->accessor->buffer_view;

    gltf_buffer * buffer = buffer_view->buffer;

    const unsigned char * buffer_data = toc->bin->data;

    if (buffer_data == toc->bin->data)
    {
	assert (buffer->byte_length <= toc->bin->length);
    }

    env->component_type = env->accessor->component_type;
    env->component_size = gltf_component_size (env->accessor->component_type);
    
    range_const_unsigned_char range_buffer = { .begin = buffer_data, .end = buffer_data + buffer->byte_length };

    range_const_unsigned_char range_buffer_view = { .begin = range_buffer.begin + buffer_view->byte_offset, .end = range_buffer_view.begin + buffer_view->byte_length };

    env->byte_stride = !buffer_view->byte_stride ? (env->component_size * env->type) : buffer_view->byte_stride;
    
    assert (env->byte_stride % (env->component_size * env->type) == 0);
    assert (env->byte_stride >= env->component_size * env->type);

    env->range.accessor.begin = range_buffer_view.begin + env->accessor->byte_offset;
    env->range.accessor.end = env->range.accessor.begin + env->accessor->count * env->byte_stride;
    
    if (range_buffer_view.begin < range_buffer.begin || range_buffer.end < range_buffer_view.end)
    {
	log_fatal ("Buffer view range is out of bounds in gltf");
    }

    assert (env->range.accessor.begin >= range_buffer_view.begin);
    assert (range_buffer_view.end >= env->range.accessor.end);
    
    if (env->range.accessor.begin < range_buffer_view.begin || range_buffer_view.end < env->range.accessor.end)
    {
	log_fatal ("Accessor range is out of bounds in gltf");
    }

    return true;
    
fail:
    return false;
}

/*bool gltf_accessor_read_tri_fvec3 (fvec3 target[3], gltf_accessor_env * env, gltf_index index)
{
    gltf_component component = { .pointer = env->range.accessor.begin + index };

#define fvec3_set_target(member, max)					\
    if ( (void*) (component.member + 9) > (void*) env->range.accessor.end ) \
    {									\
	return false;							\
    }									\
    else								\
    {									\
	target[0] = (fvec3) { .x = (fvec) component.member[0] / max, .y = (fvec) component.member[1] / max, .z = (fvec) component.member[2] / max }; \
	target[1] = (fvec3) { .x = (fvec) component.member[3] / max, .y = (fvec) component.member[4] / max, .z = (fvec) component.member[5] / max }; \
	target[2] = (fvec3) { .x = (fvec) component.member[6] / max, .y = (fvec) component.member[7] / max, .z = (fvec) component.member[8] / max }; \
	return true;							\
    }
    
    if (env->normalized)
    {
	if (env->component_type == GLTF_ACCESSOR_COMPONENT_BYTE)
	{
	    fvec3_set_target(i8, 127);
	}
	else if (env->component_type == GLTF_ACCESSOR_COMPONENT_UNSIGNED_BYTE)
	{
	    fvec3_set_target(u8, 255);
	}
	else if (env->component_type == GLTF_ACCESSOR_COMPONENT_SHORT)
	{
	    fvec3_set_target(i16, 32767);
	}
	else if (env->component_type == GLTF_ACCESSOR_COMPONENT_UNSIGNED_SHORT)
	{
	    fvec3_set_target(u16, 65535);
	}
	else
	{
	    log_fatal ("Bad type for normalized integer accessor");
	}
    }
    else
    {
	if (env->component_type == GLTF_ACCESSOR_COMPONENT_FLOAT)
	{
	    if ( (void*) (component.f + 9) > (void*) env->range.accessor.end )
	    {
		target[0] = (fvec3) { .x = component.f[0], .y = component.f[1], .z = component.f[2] };
		target[1] = (fvec3) { .x = component.f[3], .y = component.f[4], .z = component.f[5] };
		target[2] = (fvec3) { .x = component.f[6], .y = component.f[7], .z = component.f[8] };
		return true;
	    }
	    else
	    {
		return false;
	    }
	}
	else
	{
	    log_fatal ("Bad type for non-normalizd accessor");
	}
    }

fail:
    return false;
    }*/

#define define_load_from(member, max)						\
    static bool gltf_accessor_env_load_fvec3_from_##member (void * target, bool (*loader)(void * target, const fvec3 * input), range_gltf_index * indices, gltf_accessor_env * env) \
    {									\
	gltf_component component;					\
									\
	gltf_index * index;						\
									\
	fvec3 pass;							\
	assert (range_count(env->range.accessor) % sizeof(*component.member) == 0); \
									\
	for_range (index, *indices)					\
	{								\
	    component.pointer = env->range.accessor.begin + env->byte_stride * (*index); \
									\
	    if ( (void*)(component.member + 3) > (void*)env->range.accessor.end) \
	    {								\
		log_fatal ("Index to vertex attributes is out of bounds"); \
	    }								\
									\
	    pass = (fvec3)						\
		{							\
		    .x = (fvec) component.member[0] / (fvec) max,		\
		    .y = (fvec) component.member[1] / (fvec) max,		\
		    .z = (fvec) component.member[2] / (fvec) max,		\
		};							\
									\
	    if (!loader (target, &pass))				\
	    {								\
		return false;						\
	    }								\
	}								\
									\
	return true;							\
									\
    fail:								\
	return false;							\
    }									

define_load_from(u8, 255);
define_load_from(i8, 127);
define_load_from(u16, 65535);
define_load_from(i16, 32767);

static bool gltf_accessor_env_load_fvec3_from_float (void * target, bool (*loader)(void * target, const fvec3 * input), range_gltf_index * indices, gltf_accessor_env * env)
{
    gltf_component component;

    gltf_index * index;

    fvec3 pass;
    
    for_range (index, *indices)
    {
	component.pointer = env->range.accessor.begin + env->byte_stride * (*index);

	assert (range_count(env->range.accessor) % env->byte_stride == 0);
	if ( (void*)(component.f + 3) > (void*)env->range.accessor.end)
	{
	    log_fatal ("Index to vertex attributes is out of bounds %zu/%zu", *index, range_count(env->range.accessor) / env->byte_stride);
	}

	pass = (fvec3)
	    {
		.x = component.f[0],
		.y = component.f[1],
		.z = component.f[2],
	    };

	if (!loader (target, &pass))
	{
	    return false;
	}
    }

    return true;
    
fail:
    return false;
}

bool gltf_accessor_env_load_fvec3 (void * target, bool (*loader)(void * target, const fvec3 * input), range_gltf_index * indices, gltf_accessor_env * env)
{
    
    if (env->normalized)
    {
	if (env->component_type == GLTF_ACCESSOR_COMPONENT_BYTE)
	{
	    return gltf_accessor_env_load_fvec3_from_i8(target, loader, indices, env);
	}
	else if (env->component_type == GLTF_ACCESSOR_COMPONENT_UNSIGNED_BYTE)
	{
	    return gltf_accessor_env_load_fvec3_from_u8(target, loader, indices, env);
	}
	else if (env->component_type == GLTF_ACCESSOR_COMPONENT_SHORT)
	{
	    return gltf_accessor_env_load_fvec3_from_i16(target, loader, indices, env);
	}
	else if (env->component_type == GLTF_ACCESSOR_COMPONENT_UNSIGNED_SHORT)
	{
	    return gltf_accessor_env_load_fvec3_from_u16(target, loader, indices, env);
	}
	
	log_fatal ("Bad type for normalized integer accessor");
    }
    else
    {
	if (env->component_type == GLTF_ACCESSOR_COMPONENT_FLOAT)
	{
	    return gltf_accessor_env_load_fvec3_from_float(target, loader, indices, env);
	}

	log_fatal ("Bad type for non-normalizd accessor");
    }

    return true;

fail:
    return false;
}

#define define_load_indices_from(member)				\
    static void load_indices_from_##member (window_gltf_index * output, gltf_accessor_env * env) \
    {									\
	gltf_component component;					\
									\
	for_gltf_accessor(component, *env)				\
	{								\
	    *window_push(*output) = *component.member;			\
	}								\
    }

define_load_indices_from(u8);
define_load_indices_from(u16);
define_load_indices_from(u32);

bool gltf_accessor_env_load_indices (window_gltf_index * output, gltf_accessor_env * env)
{
    if (env->type != GLTF_ACCESSOR_SCALAR)
    {
	log_fatal ("Index array does not contain scalars");
    }
    
    if (env->component_type == GLTF_ACCESSOR_COMPONENT_UNSIGNED_BYTE)
    {
	load_indices_from_u8 (output, env);
    }
    else if (env->component_type == GLTF_ACCESSOR_COMPONENT_UNSIGNED_SHORT)
    {
	load_indices_from_u16 (output, env);
    }
    else if (env->component_type == GLTF_ACCESSOR_COMPONENT_UNSIGNED_INT)
    {
	load_indices_from_u32(output, env);
    }
    else
    {
	log_fatal ("Bad type for index array accessor");
    }

    return true;

fail:
    return false;
}
