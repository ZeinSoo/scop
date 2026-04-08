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

uniform Material material;

uniform sampler2D texture0;
uniform vec3 lightPos0;
uniform vec3 cameraPos;
uniform int u_debugTexture;

// Lighting using hard-coded MTL values from resources_intra/42.mtl
void newLight()
{
    // MTL: newmtl Material
    const vec3 Ka = vec3(0.0, 0.0, 0.0);
    const vec3 Kd = vec3(0.64, 0.64, 0.64);
    const vec3 Ks = vec3(0.5, 0.5, 0.5);
    const float Ns = 96.078431;
    const float d = 1;
    const int illum = 2;

    // Global ambient intensity (scene term)
    vec3 Ia = vec3(0.1);

    // World-space directions
    vec3 L = normalize(lightPos0 - vs_position);
    vec3 V = normalize(cameraPos - vs_position);

    // Flat/per-face normal reconstructed from position derivatives
    vec3 N = normalize(cross(dFdx(vs_position), dFdy(vs_position)));

    // Ambient
    vec3 ambientFinal = Ka * Ia;

    // Diffuse (Lambert)
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuseFinal = Kd * NdotL;

    // Specular (Phong)
    vec3 specularFinal = vec3(0.0);
    if (illum >= 2 && NdotL > 0.0)
    {
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(V, R), 0.0), Ns);
        specularFinal = Ks * spec;
    }

    vec3 rgb = ambientFinal + diffuseFinal + specularFinal;
    fs_color = vec4(rgb, d);
}

void newLight2()
{
    // MTL: newmtl Material
    const vec3 Ka = material.Ka;
    const vec3 Kd = material.Kd;
    const vec3 Ks = material.Ks;
    const float Ns = material.Ns;
    const float d = material.d;
    const int illum = material.illum;


    // Global ambient intensity (scene term)
    vec3 Ia = vec3(0.1f, 0.1f, 0.1f);

    // World-space directions
    vec3 L = normalize(lightPos0 - vs_position);
    vec3 V = normalize(cameraPos - vs_position);

    // Flat/per-face normal reconstructed from position derivatives
    vec3 N = normalize(cross(dFdx(vs_position), dFdy(vs_position)));

    // Ambient
    vec3 ambientFinal = Ka * Ia;

    // Diffuse (Lambert)
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuseFinal = Kd * NdotL;

    // Specular (Phong)
    vec3 specularFinal = vec3(0.0);
    if (illum >= 2 && NdotL > 0.0)
    {
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(V, R), 0.0), Ns);
        specularFinal = Ks * spec;
    }

    vec3 rgb = ambientFinal + diffuseFinal + specularFinal;
    fs_color = vec4(rgb, d) * texture(material.diffuseTex, vs_texcoord) * texture(material.specularTex, vs_texcoord);
}

void trueLight()
{
    // Ambiant Light
    vec3 ambiantLight = vec3(0.1f);

    // Diffuse Light
    vec3 posToLightDirVec = normalize(lightPos0 - vs_position);
    vec3 N = normalize(cross(dFdx(vs_position), dFdy(vs_position)));

    vec3 diffuseColor = vec3(1.f);
    float diffuse = clamp(dot(posToLightDirVec, N), 0, 1);
    vec3 diffuseFinal = diffuseColor * diffuse;

    fs_color = vec4(vs_color, 1.f)
        * (vec4(ambiantLight, 1.f) + vec4(diffuseFinal, 1.f));
}

void trueLight2()
{
    vec3 ambiantLight = vec3(0.1);

    // Directions en espace monde (vs_position est en monde chez toi)
    vec3 L = normalize(lightPos0 - vs_position);      // fragment -> light
    vec3 V = normalize(cameraPos - vs_position);      // fragment -> camera

    // Normale géométrique (flat / par face)
    vec3 N = normalize(cross(dFdx(vs_position), dFdy(vs_position)));

    // Diffuse
    float diffuse = clamp(dot(L, N), 0.0, 1.0);
    vec3 diffuseFinal = vec3(0.0) * diffuse;

    // Specular (Phong)
    vec3 R = reflect(-L, N);
    float specularConstant = pow(max(dot(V, R), 0.0), 30.0);
    vec3 specularFinal = vec3(1.0) * specularConstant;

    //fs_color = vec4(vs_color, 1.0)
    //   * (vec4(ambiantLight, 1.0) + vec4(diffuseFinal, 1.0) + vec4(specularFinal, 1.0));
    
    fs_color = texture(texture0, vs_texcoord) *
        vec4(vs_color, 1.0);
        //* (vec4(ambiantLight, 1.0) + vec4(diffuseFinal, 1.0) + vec4(specularFinal, 1.0));
}

void fakeLight()
{
    // Ambiant Light
    vec3 ambiantLight = vec3(0.1f);

    // Diffuse Light
    vec3 posToLightDirVec = normalize(lightPos0 - vs_position);
    vec3 diffuseColor = vec3(0.1f);
    float diffuse = clamp(dot(posToLightDirVec, vs_normal), 0, 1);
    vec3 diffuseFinal = diffuseColor * diffuse;

    // Specular Light
    vec3 lightToPosDirVec = normalize(vs_position - lightPos0);
    vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
    vec3 posToViewDirVec = normalize(cameraPos - vs_position);
    float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), 30);
    vec3 specularFinal = vec3(1.f) * specularConstant;

    // Attenuation

    fs_color = vec4(vs_color, 1.f)
        * (vec4(ambiantLight, 1.f) + vec4(diffuseFinal, 1.f) + vec4(specularFinal, 1.f));
}

void main()
{  
    if (u_debugTexture != 0)
    {
        fs_color = texture(texture0, vs_texcoord);
        return;
    }
    //trueLight();
    //trueLight2();
    // newLight();
    newLight2();
    //fakeLight(); 
}