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

vec3 GLOBAL_lightDirection = vec3(0.2, -0.7, 0.0);
vec3 GLOBAL_lightColor = normalize(vec3(109, 196, 199));
float ambientStr = 0.3f;


uniform vec3 CameraPosition;

uniform vec3 Ambient_Color;
uniform vec3 Diffuse_Color;
uniform vec3 Specular_Color;
uniform vec2 TexAndRim;

uniform int LightCount;
uniform sampler2D shieldTexture;

uniform int grayscale = 0;
uniform P_LIGHT pLights[LIGHTS_MAX];

vec3 calcDirLight(vec3 normal, vec3 diffuseColor);

void main() {

    vec3 position = vec3(0);
    //Create the diffuse color once
    vec3 diffuse = Diffuse_Color;
    //vec3 ambientCol = (Ambient_Color + ambientStr);
    if (TexAndRim.x == 1) {
        //ambientCol = (Ambient_Color + ambientStr) * texture(albedoTexture, f_UV).rgb;
        diffuse *= texture(shieldTexture, f_UV).rgb;
        //diffuse = Diffuse_Color;
    }

    vec3 result = diffuse;
    result += calcDirLight(f_normal, diffuse);



    //result += calcDirLight(f_normal, diffuse);
    //This is a light accumilation over the point lights


    //HOLDERS
    int grayHolder = grayscale;
    vec3 ambientHolder = Ambient_Color;
    int lightHolder = LightCount;
    vec3 cameraHolder = CameraPosition;
    //-------------------------
    vec3 viewDir = normalize(cameraHolder - f_position.xyz);
    float fresnel = 1 - dot(viewDir,  f_normal);
    result += fresnel;
    color = vec4(result, 1);
}

vec3 calcDirLight(vec3 normal, vec3 diffuseColor)
{
    float lightStr = 0.5;
    vec3 lightDir = normalize(-GLOBAL_lightDirection);
    float diff = smoothstep(0.0, 0.01, (max(dot(normal, lightDir), 0.0)));

    diffuseColor = (Diffuse_Color * texture(shieldTexture, f_UV).rgb) * diff * lightStr * GLOBAL_lightColor;

    float specularStr = 0.5;

    vec3 viewDir = normalize(CameraPosition - f_position.xyz);
    vec3 reflectDir = reflect(-lightDir, f_normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);

    vec3 specular = specularStr * spec * GLOBAL_lightColor;

    return diffuseColor + specular;
}
