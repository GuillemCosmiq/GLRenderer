#version 420 core
layout (location = 0) in vec3 aPosition;

uniform mat4 lightSpaceModelMatrix;

void main()
{
    gl_Position = lightSpaceModelMatrix * vec4(aPosition, 1.0);
}  