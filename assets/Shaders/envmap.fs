#version 330

uniform sampler2D u_Kd;
uniform samplerCube u_skybox;

in vec3 v_vertex;
in vec3 v_normal;
in vec2 v_texcoord;

out vec4 FragColor;

void main() { 
    float ratio = 0.75;
    float blend = 0.95;

    vec3 I = normalize(v_vertex);
    vec3 R = refract(I, normalize(-v_normal), ratio);

    vec4 refractColor = texture(u_skybox, R);
    vec4 textureColor = texture2D(u_Kd, v_texcoord);

    FragColor = vec4(blend * refractColor.rgb + (1.0 - blend) * textureColor.rgb, textureColor.a);
}