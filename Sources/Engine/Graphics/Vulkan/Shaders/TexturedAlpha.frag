#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define ALPHA_THRESHOLD 0.5

layout (set = 1, binding = 0) uniform sampler2D aTexture0;
layout (set = 2, binding = 0) uniform sampler2D aTexture1;
layout (set = 3, binding = 0) uniform sampler2D aTexture2;
layout (set = 4, binding = 0) uniform sampler2D aTexture3;

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec4 outColor;

void main()
{
   vec4 c = 
     texture(aTexture0, inTexCoord) * texture(aTexture1, inTexCoord) *
     texture(aTexture2, inTexCoord) * texture(aTexture3, inTexCoord) *
     inColor;

   if (c.a < ALPHA_THRESHOLD)
   {
       discard;
   }

   outColor = c;
}