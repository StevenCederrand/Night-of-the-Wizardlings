#version 430

//in vec2 f_UV;
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D brightImage;

uniform int horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);


void main(){
    
    vec2 tex_offset = 1.0 / textureSize(brightImage, 0);
    vec3 result = texture(brightImage, texCoord).rbg * weight[0];

    if (horizontal == 1)
    {   
        for (int i = 0; i < 5; i++)
        {
            result += texture(brightImage, texCoord + vec2(tex_offset.x * i, 0.0f)).rgb * weight[i];
            result += texture(brightImage, texCoord - vec2(tex_offset.x * i, 0.0f)).rgb * weight[i];
        }
        
        //FragColor = texture(brightImage, f_UV);
        //result = texture(brightImage, texCoord).rbg;
    }
    else 
    {
        
        for (int i = 1; i < 5; i++)
        {
            result += texture(brightImage, texCoord + vec2(0.0f, tex_offset.y * i)).rbg * weight[i];
            result += texture(brightImage, texCoord - vec2(0.0f, tex_offset.y * i)).rgb * weight[i];
        }
        
       // result = texture(brightImage, texCoord).rbg;
        //FragColor = vec4(1, 0, 0, 1.0);
    }
    FragColor = vec4(result, 1.0);
    
}