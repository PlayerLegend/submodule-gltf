test/glb-toc: \
	src/gltf/glb-toc.test.o \
	src/log/log.o \
	src/convert/fd/source.o \
	src/convert/source.o \
	src/window/alloc.o \
	src/gltf/convert.o \
	src/gltf/gltf.o \
	src/json/json.o \
	src/range/range_strdup_to_string.o \
	src/table/table.o

bin/glb-json: \
	src/gltf/glb-json.util.o \
	src/log/log.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/gltf/convert.o \
	src/window/alloc.o \
	src/gltf/gltf.o \
	src/json/json.o \
	src/range/range_strdup_to_string.o \
	src/table/table.o

bin/gltf-vertices: \
	src/gltf/dump-vertices.util.o \
	src/log/log.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/gltf/convert.o \
	src/window/alloc.o \
	src/gltf/gltf.o \
	src/json/json.o \
	src/range/range_strdup_to_string.o \
	src/table/table.o

bin/glb-info: \
	src/gltf/glb-info.util.o \
	src/log/log.o \
	src/json/json.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/gltf/convert.o \
	src/range/range_strdup_to_string.o \
	src/table/table.o \
	src/gltf/gltf.o

C_PROGRAMS += test/glb-toc
C_PROGRAMS += bin/glb-json
C_PROGRAMS += bin/glb-info
C_PROGRAMS += bin/gltf-vertices

glb-tests: test/glb-toc
glb-utils: bin/glb-json
glb-utils: bin/glb-info
glb-utils: bin/gltf-vertices

tests: glb-tests
utils: glb-utils

#TESTS_C += test/glb-toc
#UTILS_C += bin/glb-json bin/glb-info
