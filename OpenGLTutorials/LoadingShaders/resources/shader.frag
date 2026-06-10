#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 v_color;

void main() {
    vec4 color1 = vec4(0, 1, 0, 1);
    // out_color = vec4(v_color, 1); 
    out_color = color1; 
}