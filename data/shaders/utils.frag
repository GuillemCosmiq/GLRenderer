#version 420 core
vec3 WorldPosFromNDC(vec2 xy, float depth, mat4 invProj, mat4 invView)
{
	depth = depth * 2.0 - 1.0;
	vec4 clipSpace = vec4(xy * 2.0 - 1.0, depth, 1.0);
	vec4 viewSpace = invProj * clipSpace;
	viewSpace /= viewSpace.w;
	vec4 worldSpace = invView * viewSpace;
	return worldSpace.xyz;
}