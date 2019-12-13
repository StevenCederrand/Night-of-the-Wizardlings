#version 430

laout(loaction = 0) in vec3 vertPos;

uniform mat4 MVP;

void main (){
  gl_Position = MVP * vec4(vertPos, 1);
}
