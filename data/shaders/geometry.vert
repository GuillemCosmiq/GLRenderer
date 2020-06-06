#version 420 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
out vec4 out_position;
out vec4 out_prevPosition;
out vec3 outNormal;
out vec2 outTexCoords;
out mat3 outTBN;
layout(std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
};
uniform mat4 model;
uniform mat4 prevModel;
uniform mat3 normal;
void main()
{
vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
// re-orthogonalize T with respect to N
T = normalize(T - dot(T, N) * N);
// then retrieve perpendicular vector B with the cross product of T and N
vec3 B = cross(N, T);

//mat3 TBN = mat3(T, B, N)  

	//vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	//vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	//vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	outTBN = mat3(T, B, N);
	outNormal =  normal * aNormal;
	outTexCoords = aTexCoord;
	out_position = proj * view * model * vec4(aPosition, 1.0f);
	out_prevPosition = proj * view * prevModel * vec4(aPosition, 1.0f);
	gl_Position = out_position;
}