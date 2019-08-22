#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex;

out vec4 particleColor;

//uniform mat4 MVP;
uniform vec3 offset;
uniform vec4 color;

void main(){

	float scale = 0.005f;
    gl_Position = vec4(vertex.xyz*scale + offset, 1.0);
    particleColor = color;
}
