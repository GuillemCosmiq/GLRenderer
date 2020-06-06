#version 420 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float radius;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);    
    lightDistance = lightDistance / radius;
    gl_FragDepth = lightDistance;
}  