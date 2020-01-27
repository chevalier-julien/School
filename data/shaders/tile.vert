#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform GlobalParametersBuffer
{
	vec4 viewportSize;
} gParams;



layout(location = 0) out vec3 fragColor;

vec2 positions[4] = vec2[](
	vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5)
);

vec3 colors[4] = vec3[](
    vec3(0.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0)
);

void main()
{
	float vpRatio = gParams.viewportSize.x*gParams.viewportSize.w;

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	gl_Position.y *= vpRatio;
	
    fragColor = colors[gl_VertexIndex];
}

