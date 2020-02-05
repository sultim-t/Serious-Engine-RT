#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec4 inNormal;
layout (location = 3) in vec4 inTexCoord01;
layout (location = 4) in vec4 inTexCoord23;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outTexCoord01;
layout (location = 2) out vec4 outTexCoord23;

layout (std140, set = 0, binding = 0) uniform MainBuffer
{
	mat4 MVP;
} unBuffer;

void main() 
{
	outColor = inColor;
	outTexCoord01 = inTexCoord01;
	outTexCoord23 = inTexCoord23;

	gl_Position = unBuffer.MVP * inPosition;
}