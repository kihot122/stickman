mkdir data
glslc -O --target-env=vulkan1.2 -fshader-stage=vert src/shader/vertex.glsl -o data/vertex.spv
glslc -O --target-env=vulkan1.2 -fshader-stage=frag src/shader/fragment.glsl -o data/fragment.spv