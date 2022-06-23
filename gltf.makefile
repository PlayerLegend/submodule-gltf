test/glb-toc: \
	src/gltf/test/glb-toc.test.o \
	src/log/log.o \
	src/convert/fd/source.o \
	src/convert/source.o \
	src/window/alloc.o \
	src/json/json.o \
	src/range/strdup_to_string.o \
	src/range/streq.o \
	src/range/strdup.o \
	src/range/string_init.o \
	src/table/string.o \
	src/gltf/load/source.o \
	src/gltf/load/memory.o \

bin/glb-json: \
	src/gltf/util/glb-json.util.o \
	src/log/log.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/json/json.o \
	src/range/strdup.o \
	src/range/string_init.o \
	src/range/streq.o \
	src/range/strdup_to_string.o \
	src/table/string.o \
	src/gltf/load/source.o \
	src/gltf/load/memory.o \

bin/gltf-vertices: \
	src/convert/fd/source.o \
	src/convert/source.o \
	src/gltf/test/dump-vertices.util.o \
	src/json/json.o \
	src/log/log.o \
	src/range/strdup.o \
	src/range/strdup_to_string.o \
	src/range/streq.o \
	src/range/string_init.o \
	src/table/string.o \
	src/window/alloc.o \
	src/gltf/load/source.o \
	src/gltf/load/memory.o \

bin/glb-info: \
	src/gltf/util/glb-info.util.o \
	src/log/log.o \
	src/json/json.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/range/strdup_to_string.o \
	src/range/streq.o \
	src/range/strdup.o \
	src/range/string_init.o \
	src/table/string.o \
	src/gltf/load/source.o \
	src/gltf/load/memory.o \

test/run-gltf-vertices: src/gltf/test/dump-vertices.sh

C_PROGRAMS += test/glb-toc
C_PROGRAMS += bin/glb-json
C_PROGRAMS += bin/glb-info
C_PROGRAMS += bin/gltf-vertices
SH_PROGRAMS += test/run-gltf-vertices

gltf-tests: test/glb-toc
gltf-utils: bin/glb-json
gltf-utils: bin/glb-info
gltf-utils: bin/gltf-vertices
gltf-tests: test/run-gltf-vertices

run-tests: run-gltf-tests
run-gltf-tests:
	DEPENDS='gltf-tests gltf-utils' sh run-tests.sh test/run-gltf-vertices

depend: gltf-depend
gltf-depend:
	cdeps src/gltf > src/gltf/depends.makefile

tests: gltf-tests
utils: gltf-utils
