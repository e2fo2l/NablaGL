#version 330

uniform sampler2D u_sampler;
uniform vec3 u_color;

in vec2 v_texcoord;
out vec4 FragColor;

void main() {
    FragColor = vec4(v_texcoord.x, v_texcoord.y, 1.0, 1.0);
}