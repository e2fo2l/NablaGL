#version 330

layout(std140) uniform Camera
{
	mat4 u_viewMatrix;
	mat4 u_projectionMatrix;
};

// TODO : Use the camera's UBO object instead of this
uniform vec3 u_position;
uniform vec3 u_cameraUp;
uniform vec3 u_cameraRight;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;

out vec2 v_texcoord;

void main(void) {
    gl_Position = u_projectionMatrix * u_viewMatrix * vec4(u_position + (a_position.x * u_cameraRight) + (a_position.y * u_cameraUp), 1.0);
    v_texcoord = a_texcoord;
}