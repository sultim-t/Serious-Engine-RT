#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 1, binding = 0) uniform sampler2D aTexture0;
layout (set = 2, binding = 0) uniform sampler2D aTexture1;
layout (set = 3, binding = 0) uniform sampler2D aTexture2;
layout (set = 4, binding = 0) uniform sampler2D aTexture3;

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec4 inTexCoord01;
layout (location = 2) in vec4 inTexCoord23;

layout (location = 0) out vec4 outColor;

void main()
{
   outColor = 
     texture(aTexture0, inTexCoord01.xy) * texture(aTexture1, inTexCoord01.zw) *
     texture(aTexture2, inTexCoord23.xy) * texture(aTexture3, inTexCoord23.zw) *
     inColor;
}