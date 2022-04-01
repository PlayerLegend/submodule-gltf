test/glb-toc: \
	src/gltf/test/glb-toc.test.o \
	src/log/log.o \
	src/convert/fd/source.o \
	src/convert/source.o \
	src/window/alloc.o \
	src/gltf/convert.o \
	src/gltf/gltf.o \
	src/json/json.o \
	src/range/strdup_to_string.o \
	src/range/streq.o \
	src/range/strdup.o \
	src/range/string_init.o \
	src/table/string.o

bin/glb-json: \
	src/gltf/util/glb-json.util.o \
	src/log/log.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/gltf/convert.o \
	src/window/alloc.o \
	src/gltf/gltf.o \
	src/json/json.o \
	src/range/strdup.o \
	src/range/string_init.o \
	src/range/streq.o \
	src/range/strdup_to_string.o \
	src/table/string.o

bin/gltf-vertices: \
	src/convert/fd/source.o \
	src/convert/source.o \
	src/gltf/convert.o \
	src/gltf/test/dump-vertices.util.o \
	src/gltf/gltf.o \
	src/json/json.o \
	src/log/log.o \
	src/range/strdup.o \
	src/range/strdup_to_string.o \
	src/range/streq.o \
	src/range/string_init.o \
	src/table/string.o \
	src/window/alloc.o \

bin/glb-info: \
	src/gltf/util/glb-info.util.o \
	src/log/log.o \
	src/json/json.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/gltf/convert.o \
	src/range/strdup_to_string.o \
	src/range/streq.o \
	src/range/strdup.o \
	src/range/string_init.o \
	src/table/string.o \
	src/gltf/gltf.o

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

depend:
gltf-depend:
	sh makedepend.sh src/gltf/gltf.makefile

tests: gltf-tests
utils: gltf-utils
# DO NOT DELETE

src/gltf/gltf.o: src/gltf/def.h src/window/def.h src/range/def.h
src/gltf/gltf.o: src/range/string.h src/table/string.h src/json/def.h
src/gltf/gltf.o: src/json/parse.h src/keyargs/keyargs.h src/json/traverse.h
src/gltf/gltf.o: src/log/log.h
src/gltf/convert.o: src/gltf/convert.h src/gltf/def.h src/window/def.h
src/gltf/convert.o: src/range/def.h src/convert/status.h src/convert/source.h
src/gltf/convert.o: src/gltf/parse.h src/window/alloc.h src/log/log.h
src/gltf/util/glb-info.util.o: src/gltf/def.h src/window/def.h
src/gltf/util/glb-info.util.o: src/range/def.h src/keyargs/keyargs.h
src/gltf/util/glb-info.util.o: src/table/string.h src/json/def.h
src/gltf/util/glb-info.util.o: src/json/traverse.h src/log/log.h
src/gltf/util/glb-info.util.o: src/convert/status.h src/convert/source.h
src/gltf/util/glb-info.util.o: src/convert/fd/source.h src/gltf/convert.h
src/gltf/util/glb-json.util.o: src/gltf/convert.h src/convert/fd/source.h
src/gltf/util/glb-json.util.o: src/log/log.h
src/gltf/env.o: src/range/def.h src/window/def.h src/window/alloc.h
src/gltf/env.o: src/vec/vec.h src/vec/vec3.h src/gltf/def.h src/gltf/env.h
src/gltf/env.o: src/log/log.h
src/gltf/test/glb-toc.test.o: src/gltf/def.h src/window/def.h src/range/def.h
src/gltf/test/glb-toc.test.o: src/log/log.h src/convert/status.h
src/gltf/test/glb-toc.test.o: src/convert/source.h src/convert/fd/source.h
src/gltf/test/glb-toc.test.o: src/gltf/convert.h
src/gltf/test/dump-vertices.util.o: src/gltf/def.h src/window/def.h
src/gltf/test/dump-vertices.util.o: src/range/def.h src/log/log.h
src/gltf/test/dump-vertices.util.o: src/convert/status.h src/convert/source.h
src/gltf/test/dump-vertices.util.o: src/convert/fd/source.h
src/gltf/test/dump-vertices.util.o: src/gltf/convert.h
