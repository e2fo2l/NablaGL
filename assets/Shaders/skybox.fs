#version 330

in vec3 v_texcoords;
out vec4 FragColor;

uniform samplerCube u_sampler;

void main() {
    FragColor = texture(u_sampler, v_texcoords);
}