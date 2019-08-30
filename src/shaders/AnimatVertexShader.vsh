#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex;

out vec4 fragmentColor;

uniform mat4 MVP;
uniform vec4 color;

void main(){
    gl_Position = MVP*vec4(vertex.xyz, 1.0);
    fragmentColor = color;
}
