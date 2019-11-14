#version 430
#define LIGHTS_MAX 64

layout(std430, binding = 0) readonly buffer LightIndexBuffer {
    //Point light indexes
    int index[LIGHTS_MAX];
} lightIndexBuffer;

struct P_LIGHT {
    vec3 position;
    vec3 color; //Light that is sent out from the light
    float radius;
};

in vec2 f_UV;
in vec3 f_normal; //Comes in normalized
in vec4 f_position;

out vec4 color;
out vec4 brightColor;

vec3 GLOBAL_lightDirection = vec3(0.2f, -0.7f, 0.0f);
vec3 GLOBAL_lightColor = normalize(vec3(109, 196, 199));
float ambientStr = 0.3f;

uniform vec3 CameraPosition;

uniform vec3 Ambient_Color;
uniform vec3 Diffuse_Color;
uniform vec3 Specular_Color;
uniform vec2 TexAndRim = vec2(0, 0);

uniform int LightCount;
uniform sampler2D albedoTexture;

uniform int grayscale = 0;
uniform P_LIGHT pLights[LIGHTS_MAX];

vec3 calcPointLights(P_LIGHT pLight, vec3 normal, vec3 position, float distance);
//Calculate the directional light... Returns the diffuse color, post calculations
vec3 calcDirLight(vec3 normal, vec3 diffuseColor);
// To simulate death
vec3 grayscaleColour(vec3 col);

void main() {

    vec3 ambientCol = (Ambient_Color + ambientStr);
    if (TexAndRim.x == 1) {
        ambientCol = (Ambient_Color + ambientStr) * texture(albedoTexture, f_UV).rgb;
    }
    //vec3 position = vec3(0);
    vec3 result = ambientCol;
    //Create the diffuse color once
    vec3 diffuse = Diffuse_Color;

    if(TexAndRim.x == 1) {
        diffuse = (Diffuse_Color * texture(albedoTexture, f_UV).rgb);
    }

    result += calcDirLight(f_normal, diffuse);

    //This is a light accumilation over the point lights
    for(int i = 0; i < LightCount && lightIndexBuffer.index[i] != -1; i++) {
        uint lightIndex = lightIndexBuffer.index[i];
        //position += pLights[lightIndex].position;
        float distance = length(f_position.xyz - pLights[lightIndex].position);
        //if we are within the light position
        if(distance > pLights[lightIndex].radius) {
            continue;
        }
        else {
            result += calcPointLights(pLights[lightIndex], f_normal, f_position.xyz, distance);
        }
    }

    if(grayscale == 1){
    	result = grayscaleColour(result);
    }
    color = vec4(result, 1);
/* BLOOM STUFF
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 1.0)
        brightColor = vec4(ambientCol + result, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);*/
}
vec3 calcPointLights(P_LIGHT pLight, vec3 normal, vec3 position, float distance) {
    vec3 lightDir = normalize(pLight.position - position); //From the surface to the light
    float diff = max(dot(normal, lightDir), 0);
    vec3 ambient = vec3(0.1f) * pLight.color * ambientStr;
    vec3 diffuse = Diffuse_Color * diff;

    if(TexAndRim.x == 1) {
        diffuse = (Diffuse_Color * texture(albedoTexture, f_UV).rgb) * diff * normalize(pLight.color);
    }

    return ( diffuse);// * attenuation;
}
vec3 calcDirLight(vec3 normal, vec3 diffuseColor) {
    /* --- DIFFUSE SHADING --- */
    float lightStr = 0.5f;
    vec3 lightDir = normalize(-GLOBAL_lightDirection);
    float nDotL = dot(normal, lightDir);

    float diff = smoothstep(0.0, 0.01, (max(dot(normal, lightDir), 0.0)));
    diffuseColor = (Diffuse_Color * texture(albedoTexture, f_UV).rgb) * diff * lightStr * GLOBAL_lightColor;
    /* --- SPECULAR SHADING --- */
    float specularStr = 0.5f;

    vec3 viewDir = normalize(CameraPosition - f_position.xyz); //normalize(CameraPosition - f_position.xyz);
    vec3 reflectDir = reflect(-lightDir, f_normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    //Lock specular
    spec = smoothstep(0.005, 0.01, spec);
    vec3 specular = specularStr * spec * GLOBAL_lightColor;

    if(TexAndRim.y == 1) {
        vec3 rimColor = vec3(1.0);
        float rimThreshold = 0.1;
        float rimDot = 1 - dot(viewDir, f_normal); //Rim value
        float rimIntensity = rimDot * pow(nDotL, rimThreshold);
        rimIntensity = smoothstep(0.7 - 0.01, 0.7 - 0.01, rimIntensity);
        rimColor = diffuseColor * rimIntensity;

        return diffuseColor + specular + rimColor;
    }
    else {
        return diffuseColor + specular;
    }
}

vec3 grayscaleColour(vec3 col) {
    float colourValue = (col.r + col.g + col.b) / 3; //Calculate the average pixel colour
    return vec3(colourValue);
}
