// Basic Fragment Shader
#version 430 // minimum 430 for layout specifiers on uniforms

in vec4 vColor; // from vertex shader
in vec2 vUV;    // from vertex shader

out vec4 outputColor; // for frame buffer

layout (location = 3) uniform sampler2D mainTexture;
layout (location = 4) uniform float time;   // time since application start

void main()
{
    outputColor = texture(mainTexture, vUV);
    //outputColor.r *= sin(time);
}