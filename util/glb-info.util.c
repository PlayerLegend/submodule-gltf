#include <stdlib.h>
#include "../type.h"
#include "../../keyargs/keyargs.h"
#include "../../table/string.h"
#include "../../json/def.h"
#include "../../json/traverse.h"
#include "../../log/log.h"
#include "../../convert/type.h"
#include "../../convert/source.h"
#include "../../convert/fd/source.h"
#include <fcntl.h>
#include <stdio.h>
#include "../load/source.h"

bool print_object_string (json_object * object, const char * key, const char * prefix)
{
    json_pair * pair = json_lookup_string (object, key);

    if (pair->value.type != JSON_STRING)
    {
	log_error ("%s: does not refer to a string", key);
	return false;
    }

    log_normal ("%s %s", prefix, pair->value.string);

    return true;
}

bool print_asset (json_object * root_object)
{
    json_pair * asset_pair = json_lookup_string (root_object, "asset");

    if (!asset_pair)
    {
	log_error ("Error: asset field missing from file");
	return false;
    }

    if (asset_pair->value.type != JSON_OBJECT)
    {
	log_error ("Error: \"asset\" does not refer to a JSON object");
	return false;
    }

    json_object * asset_object = asset_pair->value.object;

    if (!print_object_string (asset_object, "generator", "Asset generator:") || !print_object_string (asset_object, "version", "GLTF version:"))
    {
	log_error ("Error within asset object");
	return false;
    }

    return true;
}

bool print_scenes (json_object * root_object)
{
    json_pair * scenes_pair = json_lookup_string(root_object, "scenes");

    if (!scenes_pair)
    {
	log_fatal ("Error: no scenes found");
    }

    if (scenes_pair->value.type != JSON_ARRAY)
    {
	log_fatal ("Error: \"scenes\" has type %s, it should be a JSON array", json_type_name(scenes_pair->value.type));
    }

    log_normal ("Object contains %zd %s:", range_count (scenes_pair->value.array), plural ("scene","scenes",range_count (scenes_pair->value.array)));

    json_pair * name_pair;
    json_value * scene_value;
    
    for_range (scene_value, scenes_pair->value.array)
    {
	if (scene_value->type != JSON_OBJECT)
	{
	    log_fatal ("Error: \"scenes\" array does not contain only JSON objects");
	}

	name_pair = json_lookup_string(scene_value->object, "name");

	if (!name_pair)
	{
	    log_fatal ("Scene name is not defined");
	}
	
	if (name_pair->value.type != JSON_STRING)
	{
	    log_fatal ("Scene name is not a string");
	    return false;
	}

	log_normal ("Scene: name=\"%s\"", name_pair->value.string);
    }

    return true;

fail:
    return false;
}

int main (int argc, char * argv[])
{
    const char * file_path;

    glb glb = {0};

    int fd;
    fd_source fd_source;

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

	if (!glb_load_source(&glb, &fd_source.source))
	{
	    log_fatal ("Could not read input file %s", file_path);
	}
	
	convert_source_clear (&fd_source.source);
        
	log_normal ("generator: %s", glb.gltf.asset.generator);
	log_normal ("version: %s", glb.gltf.asset.version);
    }

    return 0;

fail:
    return 1;
}
