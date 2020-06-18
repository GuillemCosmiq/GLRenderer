#version 420 core
layout(location = 0) out vec3 outputSample;

layout(std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
	mat4 invView;
	mat4 invProj;
};

struct GBuffer
{
	sampler2D albedo;
	highp sampler2D depth;
};
uniform GBuffer gBuffer;

uniform vec2 viewport;
uniform sampler2D sceneSample;

uniform mat4 prevProjViewMatrix;

const int numSamples = 8;

vec3 WorldPosFromNDC(vec2 xy, float depth, mat4 invProj, mat4 invView);
void main()
{
	vec2 texCoords = gl_FragCoord.xy / viewport.xy;
	// [0, 1]
	float masked = texture(gBuffer.albedo, texCoords).a;
	
	vec3 color = texture(sceneSample, texCoords).rgb;
	if (masked > 0.0)
		discard;

	float depth = texture(gBuffer.depth, texCoords).r;
	vec4 pixelWorldPos = vec4(WorldPosFromNDC(texCoords, depth, invProj, invView), 1.0);
	vec4 currentPos = proj * view * pixelWorldPos;
	currentPos /= currentPos.w;
	vec4 previousPos = prevProjViewMatrix * pixelWorldPos;
	previousPos /= previousPos.w;
 	vec2 velocity = (currentPos.xy - previousPos.xy) / 2.0; // [-1, 1]
	texCoords += velocity;

	int samplesFetched = 1;
	for(; samplesFetched < numSamples; ++samplesFetched, texCoords += velocity)
	{  
		float currentMask = texture(gBuffer.albedo, texCoords).a;
		if (currentMask > 0.0)
			break;
		vec3 currentColor = texture(sceneSample, texCoords).rgb;
		color += currentColor;
	}
	outputSample = color / samplesFetched;

	outputSample = vec3(velocity, 0);
}