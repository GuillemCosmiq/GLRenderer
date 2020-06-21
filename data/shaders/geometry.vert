#version 420 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
smooth out vec4 out_position;
smooth out vec4 out_prevPosition;
out vec3 outNormal;
out vec2 outTexCoords;
out mat3 outTBN;
layout(std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
};
uniform mat4 modelMatrix;
uniform mat4 projViewModelMatrix;
uniform mat4 projViewPrevModelMatrix;
uniform mat3 normalMatrix;
void main()
{
	vec3 T = normalize(vec3(modelMatrix * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(modelMatrix * vec4(aNormal, 0.0)));
	T = normalize(T - dot(T, N) * N); // re-orthogonalize T with respect to N
	vec3 B = cross(N, T); // then retrieve perpendicular vector B with the cross product of T and N

	outTBN = mat3(T, B, N);
	outNormal =  normalMatrix * aNormal;
	outTexCoords = aTexCoord;
	mat4 projView = proj * view;
	out_position = projViewModelMatrix * vec4(aPosition, 1.0);
	out_prevPosition = projViewPrevModelMatrix * vec4(aPosition, 1.0);
	gl_Position = out_position;
}