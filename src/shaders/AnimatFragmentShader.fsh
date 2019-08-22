#version 330 core

// Output data
out vec4 color;
in vec4 particleColor;

void main()
{
    color = particleColor;
}
