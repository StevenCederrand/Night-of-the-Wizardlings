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
uniform bool HasTex;

uniform int LightCount;
uniform sampler2D albedoTexture;

uniform int grayscale = 0;
uniform P_LIGHT pLights[LIGHTS_MAX];

void main() {

    //vec3 position = vec3(0);
    //Create the diffuse color once
    vec3 diffuse = Diffuse_Color;
    //vec3 ambientCol = (Ambient_Color + ambientStr);
    if (HasTex) {
        //ambientCol = (Ambient_Color + ambientStr) * texture(albedoTexture, f_UV).rgb;
        diffuse = (Diffuse_Color * texture(albedoTexture, f_UV).rgb);
    }
    vec3 result = diffuse;

    //result += calcDirLight(f_normal, diffuse);
    //This is a light accumilation over the point lights
   

    //HOLDERS
    int grayHolder = grayscale;
    vec3 ambientHolder = Ambient_Color;
    int lightHolder = LightCount;
    vec3 cameraHolder = CameraPosition;
    //-------------------------

    color = vec4(result, 1);
}
