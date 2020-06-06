#version 420 core
layout(location = 0) out vec3 sceneHDR;
in vec3 TexCoords;
uniform samplerCube environmentMap;
void main()
{
	sceneHDR.rgb = texture(environmentMap, TexCoords).rgb;
}