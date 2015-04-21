#version 150 core
in vec2 texcoord;
in vec2 position;
in vec3 color;
out vec3 Color;
out vec2 Texcoord;

out float tintin;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float mydata[1024];
uniform float time;
void main() {
    Color = color;
    Texcoord = texcoord;
    gl_PointSize = 0.5 + position.y*position.y*30.0;
    tintin = gl_PointSize;
    gl_Position =  model * vec4(position, 0.0 , 1.0);
}
