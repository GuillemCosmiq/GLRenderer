#version 420 core
layout(location = 0) in vec3 aPosition;
out vec3 position;
uniform mat4 proj;
uniform mat4 view;
void main()
{
	position = aPosition;
	gl_Position = proj * view * vec4(aPosition, 1.0);
}