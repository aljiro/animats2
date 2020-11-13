#version 330 core

// Output data
out vec4 color;
// input
in vec4 fragmentColor;
in vec3 normalCameraSpace;
in vec3 lightDirection;
in vec3 position;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform float alpha;

void main()
{
	float lightPower = 50.0f;
	float d = length( lightPosition - position );
	vec3 n = normalize( normalCameraSpace );
	vec3 l = normalize( lightDirection );
	float cosTheta = clamp( dot(n, l), 0, 1 );

    color = vec4(fragmentColor.xyz * lightColor * lightPower * cosTheta/(d*d), alpha);
}
