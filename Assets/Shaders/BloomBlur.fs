#version 430

out vec4 color;

uniform sampler2D bloomImage;
uniform sampler2D sceneImage;

int Grayscale = 1;

in vec2 texCoords;

vec3 toneMapping(vec3 x);
vec3 gammaCorrection(vec3 gammaColor);
vec3 grayscaleColour(vec3 col);

void main() {

    const float gamma = 2.2;
    float exposure = 1.0;

    vec3 hdrColor = texture(sceneImage, texCoords).rgb;
    vec3 bloomColor = texture(bloomImage, texCoords).rgb;
    hdrColor += bloomColor;


    vec3 result = toneMapping(hdrColor);
    result = gammaCorrection(hdrColor);
    if(Grayscale == 1) {
        result = grayscaleColour(result); 
    }
    color = vec4(result, 1.0);

}

vec3 grayscaleColour(vec3 col) {
    float colourValue = (col.r + col.g + col.b) / 3; //Calculate the average pixel colour
    return vec3(colourValue);
}

vec3 toneMapping(vec3 x) {
    float a = 6.2; //Mid
    float b = 0.5; //Toe
    float c = 1.7; //Shoulder
    float d = 0.006; //mid;
    //vec3 result = (x*(a*x+b))/(x*(a*x+c)+d);
    //Uncharted 2
    vec3 result = (x*(6.2*x+0.5))/(x*(6.2*x+1.7)+0.06);
    return result;
}

vec3 gammaCorrection(vec3 gammaColor) {
    gammaColor = pow(gammaColor, vec3(1.0/2.2));
    return gammaColor;
}
