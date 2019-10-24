#version 430

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in vec2 f_UV;
in vec3 f_normal;
in vec4 f_position;

vec3 lightDirection = vec3(0.5f, -1.0f, 0.0f);

uniform vec3 Ambient_Color;
uniform vec3 Diffuse_Color;
//uniform vec3 Specular_Color;

uniform sampler2D albedoTexture;

void main(){
    
    float ambientStr = 0.15f;
    vec3 ambientCol = (Ambient_Color + ambientStr) * texture(albedoTexture, f_UV).rgb;

    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(f_normal, lightDir), 0.0f);
    vec3 diffuse = (Diffuse_Color * texture(albedoTexture, f_UV).rgb) * diff;
   // Specular_Color;
    //FragColor = texture(albedoTexture, f_UV); 
    // transforming the fragment into grayscale 
    float brightness = dot(diffuse, vec3(0.2126, 0.7152, 0.0722));
    
    if(brightness > 0.50)
        BrightColor = vec4(ambientCol + diffuse, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    
    FragColor = texture(albedoTexture, f_UV); 
    FragColor = vec4(ambientCol + diffuse, 1);
    
}
