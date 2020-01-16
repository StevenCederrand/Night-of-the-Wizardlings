#version 430
#define LIGHTS_MAX 64

in vec2 f_UV;
in vec3 f_normal; //Comes in normalized
in vec4 f_position;

out vec4 color;
out vec4 brightColor;

uniform vec3 CameraPosition;

uniform vec3 Ambient_Color;
uniform vec3 Diffuse_Color;
uniform vec3 Specular_Color;
uniform vec2 TexAndRim;
uniform float time;

uniform sampler2D shieldTexture;

vec2 rotate(float magnitude, vec2 p);
void main() {
    vec3 pivot = vec3(0.5, -0.5, 1.);
    vec2 p = f_UV - pivot.xy;
    p = rotate(3.14 * time * 0.1, p);
    float a = atan(p.y, p.x) * 1;
    float r = sqrt(dot(p,p));
    vec2 finalUV;
    finalUV.x = (time * -.5) - 1/(r + 1.7);
    finalUV.y = pivot.z * a/3.1416;

    //Create the diffuse color once
    vec3 diffuse = Diffuse_Color;
    vec4 alphaTexture = texture(shieldTexture, finalUV);

    //vec3 ambientCol = (Ambient_Color + ambientStr);
    if (TexAndRim.x == 1) {
        diffuse *= alphaTexture.rgb;
    }

    vec3 result = diffuse;

    //HOLDERS (Values that we take in but not currently use: aka CLEAN UP)
    vec3 ambientHolder = Ambient_Color;
    vec3 cameraHolder = CameraPosition;

    result += (Ambient_Color * 0.0001f);
    //-------------------------

    vec3 viewDir = normalize(cameraHolder - f_position.xyz);
    float fresnel = 1 - dot(viewDir,  f_normal);
    result += fresnel;
    color = vec4(result, alphaTexture.a * 1.8);
}

vec2 rotate(float magnitude, vec2 p)
{
    float sinTheta = sin(magnitude);
    float cosTheta = cos(magnitude);
    mat2 rotationMat = mat2(cosTheta, -sinTheta, sinTheta, cosTheta);
    return p * rotationMat;
}
