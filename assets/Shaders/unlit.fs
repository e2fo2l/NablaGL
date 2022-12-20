#version 330

uniform sampler2D u_sampler;
uniform vec3 u_color;

in vec2 v_texcoord;
out vec4 FragColor;

void main() {
    FragColor = texture2D(u_sampler, v_texcoord);
}