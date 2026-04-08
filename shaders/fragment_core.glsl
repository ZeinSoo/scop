#version 460

struct Material {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Ns; // Shininess
    float d;  // Opacity
    int illum; // Illumination model

    sampler2D diffuseTex;
    sampler2D specularTex;
};


in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;


// Uniforms
uniform Material material;

uniform vec3 lightPos0;
uniform vec3 cameraPos;

uniform int u_debugTexture;
uniform int u_debugColor;


// Functions

vec3 calculateAmbient(Material mat, vec3 Ia) {
    return mat.Ka * Ia;
}

vec3 calculateDiffuse(Material mat, vec3 N, vec3 L) {
    float NdotL = max(dot(N, L), 0.0);
    return mat.Kd * NdotL;

}

vec3 calculateSpecular(Material mat, vec3 N, vec3 L, vec3 V) {
    if (mat.illum >= 2 && max(dot(N, L), 0.0) > 0.0) {
        vec3 R = reflect(-L, N); // Reflection direction
        float RdotV = max(dot(R, V), 0.0);
        return mat.Ks * pow(RdotV, mat.Ns);
    }
    return vec3(0.0);
}

void light() {

    // Ambiant light
    vec3 Ia = vec3(0.1f); // Global ambient intensity (scene term)
    vec3 ambientFinal = calculateAmbient(material, Ia);

    // World-space directions
    vec3 L = normalize(lightPos0 - vs_position);
    vec3 V = normalize(cameraPos - vs_position);

    // Flat normal reconstructed from position derivatives
    vec3 N = normalize(cross(dFdx(vs_position), dFdy(vs_position)));

    // Diffuse (Lambertian reflectance)
    vec3 diffuseFinal = calculateDiffuse(material, N, L);

    // Specular (Phong reflection model)
    vec3 specularFinal = calculateSpecular(material, N, L, V);

    vec3 rgb = ambientFinal + diffuseFinal + specularFinal;
    fs_color = vec4(rgb, material.d) * texture(material.diffuseTex, vs_texcoord);
}

// Source : https://en.wikipedia.org/wiki/Phong_reflection_model
void main() {

    if (u_debugTexture != 0) {
        fs_color = texture(material.diffuseTex, vs_texcoord);
        return;
    }
    else if (u_debugColor != 0) {
        fs_color = vec4(vs_color, 1.0);
        return;
    }
    else {
        light();
    }
}