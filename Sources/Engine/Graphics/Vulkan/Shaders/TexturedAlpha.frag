#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define ALPHA_THRESHOLD 0.5

//layout (set = 0, binding = 1) uniform sampler2D mainTexture;

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec4 outColor;

void main()
{
   // vec4 c = texture(mainTexture, inTexCoord) * inColor;
   vec4 c = inColor;

   if (c.a < ALPHA_THRESHOLD)
   {
       discard;
   }

   outColor = inColor;
}