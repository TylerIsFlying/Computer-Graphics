// --------- VERTEX ATTRIBUTES  ---------
#version 430 // minimum 430 for layout specifiers on uniforms
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec4 normal;
// --------- UNIFORM  ---------
layout (location = 0) uniform mat4 proj;    // proj
layout (location = 1) uniform mat4 view;    // view
layout (location = 2) uniform mat4 model;   // model

layout (location = 4) uniform float time;   // time since application start
// --------- SENDS  ---------
out vec2 vUV;       // output to later stages
out vec3 vNormal;

void main()
{
    vec4 finalPosition = position;

    gl_Position = proj * view * model * finalPosition;
	
    vUV = uv;
	vNormal = (model * normal).xyz;
};