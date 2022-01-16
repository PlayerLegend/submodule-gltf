#ifndef FLAT_INCLUDES
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#endif

#define GLB_MAGIC 0x46546C67
#define GLB_CHUNKTYPE_JSON 0x4E4F534A
#define GLB_CHUNKTYPE_BIN 0x004E4942

typedef struct glb_header glb_header;
struct glb_header {
    uint32_t magic;
    uint32_t version;
    uint32_t length;
};

typedef struct glb_chunk_header glb_chunk_header;
struct glb_chunk_header
{
    uint32_t length;
    uint32_t type;
    unsigned char data[];
};

typedef struct glb_toc glb_toc;
struct glb_toc
{
    const glb_header * header;
    const glb_chunk_header * json;
    const glb_chunk_header * bin;
};

typedef struct gltf_buffer gltf_buffer;
struct gltf_buffer {
    char * uri;
    size_t byte_length;
};

typedef struct gltf_buffer_view gltf_buffer_view;
struct gltf_buffer_view {
    gltf_buffer * buffer;
    size_t byte_length;
    size_t byte_offset;
    size_t byte_stride;
};

typedef enum gltf_accessor_type gltf_accessor_type;
enum gltf_accessor_type {
    GLTF_ACCESSOR_BADTYPE = 0,
    GLTF_ACCESSOR_SCALAR = 1,
    GLTF_ACCESSOR_VEC2 = 2,
    GLTF_ACCESSOR_VEC3 = 3,
    GLTF_ACCESSOR_VEC4 = 4,
    GLTF_ACCESSOR_MAT2 = 4,
    GLTF_ACCESSOR_MAT3 = 9,
    GLTF_ACCESSOR_MAT4 = 16,
};

typedef enum gltf_accessor_component_type gltf_accessor_component_type;
enum gltf_accessor_component_type {
    GLTF_ACCESSOR_COMPONENT_BADTYPE = 0,
    GLTF_ACCESSOR_COMPONENT_BYTE = 5120,
    GLTF_ACCESSOR_COMPONENT_UNSIGNED_BYTE = 5121,
    GLTF_ACCESSOR_COMPONENT_SHORT = 5122,
    GLTF_ACCESSOR_COMPONENT_UNSIGNED_SHORT = 5123,
    GLTF_ACCESSOR_COMPONENT_UNSIGNED_INT = 5125,
    GLTF_ACCESSOR_COMPONENT_FLOAT = 5126,
};

typedef struct gltf_asset gltf_asset;
struct gltf_asset {
    char * generator;
    char * version;
};

typedef struct gltf_accessor gltf_accessor;
struct gltf_accessor {
    gltf_accessor_type type;
    gltf_accessor_component_type component_type;
    gltf_buffer_view * buffer_view;
    size_t byte_offset;
    size_t count;
    bool normalized;
    struct {
	bool present;
	size_t count;
	struct {
	    gltf_buffer_view * buffer_view;
	    size_t byte_offset;
	    gltf_accessor_component_type component_type;
	}
	    indices;
	struct {
	    gltf_buffer_view * buffer_view;
	    size_t byte_offset;
	}
	    values;
    }
	sparse;
};

typedef struct gltf_material gltf_material;
struct gltf_material {
    char * name;
    bool double_sided;
    double metallic_factor;
    double roughness_factor;
};

typedef struct gltf_mesh_primitive_target gltf_mesh_primitive_target;
struct gltf_mesh_primitive_target {
    gltf_accessor *position, *normal, *tangent;
};

typedef struct gltf_mesh_primitive gltf_mesh_primitive;
struct gltf_mesh_primitive {
    struct {
	gltf_accessor *position, *normal, *tangent;
	struct range(gltf_accessor*) texcoord, color, joints, weights;
    }
	attributes;

    struct range (gltf_mesh_primitive_target) targets;
    
    gltf_accessor * indices;
    gltf_material * material;
};

typedef struct gltf_mesh gltf_mesh;
struct gltf_mesh {
    char * name;
    struct range (gltf_mesh_primitive) primitives;
};

typedef struct gltf gltf;
struct gltf {
    gltf_asset asset;
    struct range(gltf_buffer) buffers;
    struct range(gltf_buffer_view) buffer_views;
    struct range(gltf_accessor) accessors;
    struct range(gltf_material) materials;
    struct range(gltf_mesh) meshes;
};

typedef uint32_t gltf_index;

typedef union {
    union {
	const int8_t * i8;
	const uint8_t * u8;
	const int16_t * i16;
	const uint16_t * u16;
	const uint32_t * u32;
	const gltf_index * index;
	const float * f;
    };
    const unsigned char * pointer;
}
    gltf_component;

inline static size_t gltf_component_size (gltf_accessor_component_type type)
{
    switch (type)
    {
    default: return -1;
    case GLTF_ACCESSOR_COMPONENT_BYTE: return 1;
    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_BYTE: return 1;
    case GLTF_ACCESSOR_COMPONENT_SHORT: return 2;
    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_SHORT: return 2;
    case GLTF_ACCESSOR_COMPONENT_UNSIGNED_INT: return 4;
    case GLTF_ACCESSOR_COMPONENT_FLOAT: return 4;
    }
}

typedef struct {
    gltf_accessor * accessor;
    gltf_accessor_type type;
    gltf_accessor_component_type component_type;
    size_t component_size;
    size_t byte_stride;
    bool normalized;
    struct {
	range_const_unsigned_char accessor;
    }
	range;
}
    gltf_accessor_env;

range_typedef(gltf_index, gltf_index);
window_typedef(gltf_index, gltf_index);

#define for_gltf_accessor(component, accessor_env)	\
    for ((component).pointer = (accessor_env).range.accessor.begin; (component).pointer < (accessor_env).range.accessor.end; (component).pointer += (accessor_env).byte_stride)
