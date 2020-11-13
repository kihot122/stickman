glslc -O --target-env=vulkan1.2 -fshader-stage=vert vertex.glsl -o vertex.spv
glslc -O --target-env=vulkan1.2 -fshader-stage=frag fragment.glsl -o fragment.spv