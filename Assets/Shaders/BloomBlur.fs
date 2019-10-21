#version 430

out vec4 color;

uniform sampler2D bloomImage;
uniform sampler2D sceneImage;

in vec2 texCoords;

void main() {

    const float gamma = 1.5;
    float exposure = 0.5;

    vec3 hdrColor = texture(sceneImage, texCoords).rgb;
    vec3 bloomColor = texture(bloomImage, texCoords).rgb;
    hdrColor += bloomColor;

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    result = pow(result, vec3(1.0 / gamma));

    color = vec4(result, 1.0);

}
