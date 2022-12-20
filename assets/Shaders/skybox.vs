#version 330

layout(std140) uniform Camera
{
	mat4 u_viewMatrix;
	mat4 u_projectionMatrix;
};

in vec3 a_position;
out vec3 v_texcoords;

void main(void) {
    mat4 view = mat4(mat3(u_viewMatrix)); // Remove translation component
    vec4 pos = u_projectionMatrix * view * vec4(a_position, 1.0);
    gl_Position = pos.xyww; // hack: normalized device coordinates
    v_texcoords = a_position;
}