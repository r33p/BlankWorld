#version 150 core
in vec2 texcoord;
in vec2 position;
in vec3 color;
out vec3 Color;
out vec2 Texcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float mydata[1024];
void main() {
    Color = color;
    Texcoord = texcoord;
    gl_PointSize = position.y*position.y*30.0;
    gl_Position = model * vec4(position, 0.0 , 1.0);
}
