#version 330

layout(std140) uniform Camera
{
	mat4 u_viewMatrix;
	mat4 u_projectionMatrix;
};

uniform mat4 u_modelMatrix;
uniform mat4 u_normalMatrix;

layout(location = 0) in vec3 a_vertex;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

out vec3 v_vertex;
out vec3 v_normal;
out vec2 v_texcoord;

void main(void) {
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_vertex, 1.0);
    vec4 vertex4 = u_modelMatrix * vec4(a_vertex, 1.0);
    v_vertex = vec3(vertex4) / vertex4.w;
    v_normal = mat3(u_normalMatrix) * a_normal;
    v_texcoord = a_texcoord;
}