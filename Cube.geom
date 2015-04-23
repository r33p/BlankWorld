#version 150 core

in float tintin[];

layout(lines) in;
layout(triangle_strip, max_vertices = 6) out;

void main() {
    
    gl_Position = gl_in[0].gl_Position + vec4(-0.01,0.0, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.01, 0.0, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.001*tintin[0], 0.0, 0.0);
    EmitVertex();
    EndPrimitive();
    
    gl_Position = gl_in[0].gl_Position + vec4(-0.01,0.0, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.01, 0.0, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.001*tintin[0], 0.0);
    EmitVertex();
    EndPrimitive();
    
}
