#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform GlobalParameters
{
	vec4 viewportSize;
} gParams;

layout(set = 1, binding = 0) uniform TileSetInfos
{
	uvec2 sizes;	// tile size, set size
} gTileSet;

layout(set = 1, binding = 1) uniform TileSetInstanceInfos
{
	vec4 position_scale;
} gInstance;

struct TileData
{
	uint id;
	vec4 offset_scale;
};

layout(set = 1, binding = 2) readonly buffer TileBufferLayout {
	TileData TileBuffer[];
};


layout(location = 0) out vec2 texCoords;
layout(location = 1) out vec3 fragColor;

vec2 offsets[4] = vec2[](
	vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0)
);

vec2 uvs[4] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
	vec2(1.0, 1.0)
);

vec3 colors[4] = vec3[](
    vec3(0.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0)
);

void main()
{
	TileData tileData = TileBuffer[gl_InstanceIndex];

	vec2 p = offsets[gl_VertexIndex] * tileData.offset_scale.zw * gTileSet.sizes.x + tileData.offset_scale.xy;
	p = p * gInstance.position_scale.zw + gInstance.position_scale.xy;

	uvec2 tileCoord = uvec2(tileData.id%gTileSet.sizes.y, tileData.id/gTileSet.sizes.y);
	vec2 uv = (vec2(tileCoord) + offsets[gl_VertexIndex]) / float(gTileSet.sizes.y);

	//////
	
    gl_Position = vec4(p * gParams.viewportSize.zw * 2.0 - vec2(1.0), 0.0, 1.0);
	
	texCoords = uv;
    fragColor = colors[gl_VertexIndex];
}

