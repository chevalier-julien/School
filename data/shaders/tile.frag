#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(set = 1, binding = 3) uniform sampler2D TileSetTexture;


layout(location = 0) in vec2 texCoords;
layout(location = 1) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(TileSetTexture, texCoords);
}
