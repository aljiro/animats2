#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

out vec4 fragmentColor;
out vec3 normalCameraSpace;
out vec3 lightDirection;
out vec3 position;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec4 color;
uniform vec3 lightPosition;

void main(){	
    gl_Position = MVP*vec4(vertex.xyz, 1.0);

    // Computing the normal in camera space;
    vec3 posWorld = (M * vec4(vertex.xyz, 0.0)).xyz;
    vec3 posCameraSpace = (V * M * vec4(vertex.xyz, 0.0)).xyz;
    normalCameraSpace = (V * M * vec4(normal.xyz, 0.0)).xyz;

    //Computing the light direction
    vec3 eyeDirection = vec3(0,0,0) - posCameraSpace;
    vec3 lightPositionCameraSpace = (V * vec4(lightPosition.xyz, 0.0)).xyz;
    lightDirection = lightPositionCameraSpace + eyeDirection; 

    fragmentColor = color;
    position = posWorld;
}
