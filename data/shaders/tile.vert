#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform GlobalParameters
{
	vec4 viewportSize;
} gParams;

layout(set = 1, binding = 0) uniform TileSetParameters
{
	uint size;
} gTileSet;

layout(set = 2, binding = 0) uniform TileSetInstanceInfos
{
	vec2 position;
} gInstanceInfos;


struct TileSetInstanceData
{
	uint id;
	vec2 position;
};

/*const TileSetInstanceData InstanceBuffer[] = {
	{0, vec2(0.0, 0.0)},
	{0, vec2(256.0, 0.0)},
	{0, vec2(0.0, 256.0)},
	{0, vec2(256.0, 256.0)}
};*/

layout(set = 2, binding = 1) buffer InstanceBufferLayout {
	TileSetInstanceData InstanceBuffer[];
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
	TileSetInstanceData tileData = InstanceBuffer[gl_InstanceIndex];
	
	const float TileSize = 256.0;
	
	vec2 p = tileData.position + offsets[gl_VertexIndex] * TileSize;
	p += gInstanceInfos.position;


	uvec2 tileCoord = uvec2(tileData.id%gTileSet.size, tileData.id/gTileSet.size);
	vec2 uv = vec2(tileCoord) + offsets[gl_VertexIndex] / float(gTileSet.size);

	//////
	
    gl_Position = vec4(p * gParams.viewportSize.zw * 2.0 - vec2(1.0), 0.0, 1.0);
	
	texCoords = uv;
    fragColor = colors[gl_VertexIndex];
}

