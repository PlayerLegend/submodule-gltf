test/glb-toc: src/gltf/glb-toc.test.o
test/glb-toc: src/gltf/gltf.o
test/glb-toc: src/log/log.o
test/glb-toc: src/json/json.o
test/glb-toc: src/convert/def.o
test/glb-toc: src/convert/fd.o
test/glb-toc: src/window/alloc.o
test/glb-toc: src/gltf/convert.o
test/glb-toc: src/table/table.o

bin/glb-json: src/gltf/glb-json.util.o
bin/glb-json: src/gltf/gltf.o
bin/glb-json: src/log/log.o
bin/glb-json: src/json/json.o
bin/glb-json: src/convert/fd.o
bin/glb-json: src/gltf/convert.o
bin/glb-json: src/window/alloc.o
bin/glb-json: src/table/table.o
bin/glb-json: src/convert/def.o

bin/gltf-vertices: src/gltf/dump-vertices.util.o
bin/gltf-vertices: src/gltf/gltf.o
bin/gltf-vertices: src/log/log.o
bin/gltf-vertices: src/json/json.o
bin/gltf-vertices: src/convert/fd.o
bin/gltf-vertices: src/gltf/convert.o
bin/gltf-vertices: src/window/alloc.o
bin/gltf-vertices: src/table/table.o
bin/gltf-vertices: src/convert/def.o

bin/glb-info: src/gltf/glb-info.util.o
bin/glb-info: src/gltf/gltf.o
bin/glb-info: src/log/log.o
bin/glb-info: src/json/json.o
bin/glb-info: src/convert/fd.o
bin/glb-info: src/gltf/convert.o
bin/glb-info: src/window/alloc.o
bin/glb-info: src/table/table.o
bin/glb-info: src/convert/def.o

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
