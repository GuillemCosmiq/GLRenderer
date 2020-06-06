#version 420 core
layout(location = 0) in vec3 aPosition;
layout(std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
};
out vec3 TexCoords;
void main()
{
	TexCoords = aPosition;
	vec4 pos = proj * mat4(mat3(view)) * vec4(aPosition, 1.0);
	gl_Position = pos.xyww;
}