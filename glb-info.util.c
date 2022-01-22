#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../keyargs/keyargs.h"
#include "../json/def.h"
#include "../json/traverse.h"
#include "../window/def.h"
#include "def.h"
#include "../log/log.h"
#include "../convert/source.h"
#include "convert.h"
#include "../convert/fd/source.h"

bool print_object_string (json_object * object, const char * key, const char * prefix)
{
    json_value * value = json_lookup (object, key);

    if (value->type != JSON_STRING)
    {
	log_error ("%s: does not refer to a string", key);
	return false;
    }

    log_normal ("%s %s", prefix, value->string);

    return true;
}

bool print_asset (json_object * root_object)
{
    json_value * asset_value = json_lookup (root_object, "asset");

    if (!asset_value)
    {
	log_error ("Error: asset field missing from file");
	return false;
    }

    if (!asset_value)
    {
	log_error ("Error: Asset object not found in JSON");
	return false;
    }

    if (asset_value->type != JSON_OBJECT)
    {
	log_error ("Error: \"asset\" does not refer to a JSON object");
	return false;
    }

    json_object * asset_object = asset_value->object;

    if (!print_object_string (asset_object, "generator", "Asset generator:") || !print_object_string (asset_object, "version", "GLTF version:"))
    {
	log_error ("Error within asset object");
	return false;
    }

    return true;
}

bool print_scenes (json_object * root_object)
{
    json_value * scenes_value = json_lookup(root_object, "scenes");

    if (!scenes_value)
    {
	log_error ("Error: no scenes found");
	return false;
    }

    if (scenes_value->type != JSON_ARRAY)
    {
	log_error ("Error: \"scenes\" has type %s, it should be a JSON array", json_type_name(scenes_value->type));
	return false;
    }

    log_normal ("Object contains %zd %s:", range_count (scenes_value->array), plural ("scene","scenes",range_count (scenes_value->array)));

    json_value * name_value;
    json_value * scene_value;
    
    for_range (scene_value, scenes_value->array)
    {
	if (scene_value->type != JSON_OBJECT)
	{
	    log_error ("Error: \"scenes\" array does not contain only JSON objects");
	    return false;
	}

	name_value = json_lookup(scene_value->object, "name");

	if (name_value->type != JSON_STRING)
	{
	    log_error ("Error: scene name is not a string");
	    return false;
	}

	log_normal ("Scene: name=\"%s\"", name_value->string);
    }

    return true;
}

int main (int argc, char * argv[])
{
    const char * file_path;

    glb_toc toc;

    int fd;
    fd_source fd_source;

    gltf gltf;

    window_unsigned_char buffer = {0};
    
    for (int i = 1; i < argc; i++)
    {
	file_path = argv[i];

	if (argc > 2)
	{
	    log_normal ("%s:", file_path);
	}

	fd = open (file_path, O_RDONLY);

	if (fd < 0)
	{
	    perror (file_path);
	    log_fatal ("Could not open input file %s", file_path);
	}

	fd_source = fd_source_init (fd, &buffer);

	if (!gltf_load_from_source(&gltf, &toc, &fd_source.source))
	{
	    log_fatal ("Could not read input file %s", file_path);
	}
	
	convert_source_clear (&fd_source.source);
        
	log_normal ("generator: %s", gltf.asset.generator);
	log_normal ("version: %s", gltf.asset.version);
    }

    return 0;

fail:
    return 1;
}
