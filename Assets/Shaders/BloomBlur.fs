#version 430

out vec4 color;

uniform sampler2D bloomImage;
uniform sampler2D sceneImage;

in vec2 texCoords;

vec3 custom(vec3 x)
{
    float a = 6.2; //Mid
    float b = 0.5; //Toe
    float c = 1.7; //Shoulder
    float d = 0.006; //mid;

    //vec3 result = (x*(a*x+b))/(x*(a*x+c)+d);
    //Uncharted 2
    vec3 result = (x*(6.2*x+0.5))/(x*(6.2*x+1.7)+0.06);
    return result;
}

vec3 gammaCorrection(vec3 gammaColor)
{
    gammaColor = pow(gammaColor, vec3(1.0/2.2)); 
    return gammaColor;
}

void main() {

    const float gamma = 2.2;
    float exposure = 1.0;

    vec3 hdrColor = texture(sceneImage, texCoords).rgb;
    vec3 bloomColor = texture(bloomImage, texCoords).rgb;
    hdrColor += bloomColor;

    //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    //result = pow(result, vec3(1.0 / gamma));

    vec3 result = custom(hdrColor);
    result = gammaCorrection(hdrColor);

    color = vec4(result, 1.0);

}
