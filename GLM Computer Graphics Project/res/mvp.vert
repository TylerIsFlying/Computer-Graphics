#version 430
// -------- VERTEX ATTRIBUTES -------- 
layout (location = 0) in vec4 position;     // in from vertex data
layout (location = 1) in vec4 color;        // in from vertex data
layout (location = 2) in vec2 uv;           // in from vertex data

// -------- UNIFORMS -------- 
layout (location = 0) uniform mat4 proj;    // proj
layout (location = 1) uniform mat4 view;    // view
layout (location = 2) uniform mat4 model;   // model

layout (location = 4) uniform float time;   // time since application start

// --------- SENDS ------------
out vec4 vColor;    // output to later stages
out vec2 vUV;       // output to later stages

void main()
{
    vec4 finalPosition = position;
    //finalPosition.x += sin(position.y + time) * 1.5f;
    //finalPosition.y += sin(position.z + time) * 2.5f;
    //finalPosition.z += sin(position.x + time) * 0.5f;

    gl_Position = proj * view * model * finalPosition;
    vColor = color;
    vUV = uv;
};