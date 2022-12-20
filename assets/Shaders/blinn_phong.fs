#version 330

layout(std140) uniform Light
{
    vec3 u_lightPosition;
    float u_ambientIntensity;
    vec3 u_lightColor;
    float u_specularIntensity;
};

// Material
uniform vec3 u_Ka; // Ambient color
uniform sampler2D u_Kd; // Diffuse color
uniform vec3 u_Ks; // Specular color
uniform float u_Ns; // Material shininess

in vec3 v_vertex;
in vec3 v_normal;
in vec2 v_texcoord;

out vec4 FragColor;

void main() {
    float Id = 1.0; // Diffuse intensity

    vec3 N = normalize(v_normal);
    vec3 L = normalize(u_lightPosition - v_vertex);
    float lambert = max(dot(N, L), 0.0);
    
    vec3 V = normalize(-v_vertex);
    float specular = 0.0;
    if(lambert > 0.0) {
        vec3 halfVec = normalize(V + L);
        specular = pow(max(dot(halfVec, N), 0.0), u_Ns);
    }

    vec4 txcolor = texture2D(u_Kd, v_texcoord);      
    vec3 frag = pow(u_ambientIntensity * u_Ka, vec3(2.2)) * txcolor.rgb + // Ambient
                Id * lambert * txcolor.rgb + // Diffuse
                pow(u_specularIntensity * specular * u_Ks, vec3(2.2)); // Specular
    FragColor = vec4(frag * pow(u_lightColor, vec3(2.2)), txcolor.a);
}