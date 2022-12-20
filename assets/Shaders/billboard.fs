#version 330

layout(std140) uniform Light
{
    vec3 u_lightPosition;
    float u_ambientIntensity;
    vec3 u_lightColor;
    float u_specularIntensity;
};

uniform sampler2D u_sampler;

in vec2 v_texcoord;
out vec4 FragColor;

void main() {
    FragColor = vec4(u_lightColor, 1.0) * texture2D(u_sampler, v_texcoord);
}