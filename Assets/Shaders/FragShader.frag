#version 430
#define LIGHTS_MAX 64


layout(std430, binding = 0) readonly buffer LightIndexBuffer {
    //Point light indexes
    int index[LIGHTS_MAX];
} lightIndexBuffer;

struct P_LIGHT {
    vec3 position;
    vec3 attenuation;
    float radius;
};

in vec2 f_UV;
in vec3 f_normal;
in vec4 f_position;
out vec4 color;

vec3 lightDirection = vec3(0.5f, -1.0f, 0.0f);
vec3 lightCol = vec3(1);
float ambientStr = 1.0f;

uniform vec3 Ambient_Color;
uniform vec3 Diffuse_Color;
uniform vec3 Specular_Color;
uniform bool HasTex;
uniform int LightCount;
uniform sampler2D albedoTexture;

uniform P_LIGHT pLights[LIGHTS_MAX];

vec3 calcLights(P_LIGHT pLight, vec3 normal, vec3 position, float distance, vec3 lightPosition) {
    vec3 lightDir = normalize(lightPosition - position);
    float diff = max(dot(normal, lightDir), 0);
    vec3 ambient = vec3(0.1f) * lightCol * ambientStr;
    vec3 diffuse = Diffuse_Color * diff;
    if(HasTex)
        diffuse = (Diffuse_Color * texture(albedoTexture, f_UV).rgb) * diff;

    float attenuation = 1 / (pLight.attenuation.x + pLight.attenuation.y * distance + pLight.attenuation.z * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    //Specular_Color *= 0;
    //vec3 spec = Specular_Color * 0;

    return (ambient + diffuse) * attenuation;
}


//Calculate the directional light... Returns the diffuse color, post calculations
vec3 calcDirLight(vec3 lightDirection, vec3 normal, vec3 diffuseColor);

void main() {
    float ambientStr = 0.1f;
    vec3 ambientCol = (Ambient_Color + ambientStr);

    if (HasTex)
        ambientCol = (Ambient_Color + ambientStr) * texture(albedoTexture, f_UV).rgb;

    vec3 position = vec3(0);
    vec3 result = ambientCol;

    //Create the diffuse color once
    vec3 diffuse = Diffuse_Color;
    if(HasTex)
        diffuse = (Diffuse_Color * texture(albedoTexture, f_UV).rgb);

    result += calcDirLight(vec3(0.2, -0.2, 0.0), f_normal, diffuse);

    //This is a light accumilation over the point lights
    for(int i = 0; i < LightCount && lightIndexBuffer.index[i] != -1; i++) {
        uint lightIndex = lightIndexBuffer.index[i];
        position += pLights[lightIndex].position;

        float distance = length(f_position.xyz - pLights[lightIndex].position);
        //if we are within the light position
        if(distance < pLights[lightIndex].radius) {
            result += calcLights(pLights[lightIndex], f_normal, f_position.xyz, distance, position);
        }
        else {
            //result += ambientCol;
        }
        position = vec3(0);
    }


    color = vec4(result, 1);
}

vec3 calcDirLight(vec3 lightDirection, vec3 normal, vec3 diffuseColor) {
    float lightStr = 0.1f;
    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(normal, lightDir), 0.0);

    diffuseColor = (Diffuse_Color * texture(albedoTexture, f_UV).rgb) * diff * lightStr;
    return diffuseColor;
}
