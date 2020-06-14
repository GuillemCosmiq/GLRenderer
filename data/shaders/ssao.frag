#version 420 core
layout(location = 0) out vec3 ssao;
layout(std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
	mat4 invView;
	mat4 invProj;
};
struct GBuffer
{
	highp sampler2D normals;
	highp sampler2D depth;
};
uniform GBuffer gBuffer;
uniform vec2 viewport;
uniform sampler2D noiseTexture;

const float radius = 1.0;
const int kernelSize = 64;
uniform vec3 samples[kernelSize];

uniform mat4 uNormalViewMatrix;

float linearizeDepth(float near, float far, float depth) {
    return 2.0 * near / (far + near - depth * (far - near));
}

vec3 ViewPosFromNDC(vec2 xy, float depth, mat4 invProj);
void main()
{
	vec2 screenUVs = gl_FragCoord.xy / viewport;
	float depth = texture(gBuffer.depth, screenUVs).r;
      
    vec3 viewSpacePos = ViewPosFromNDC(screenUVs, depth, invProj);

	vec3 normals = texture(gBuffer.normals, screenUVs).rgb;
	vec2 noiseScale = vec2(viewport.x/ 4.0, viewport.y/ 4.0);
	vec3 randomVec = texture(noiseTexture, screenUVs * noiseScale).xyz;

	vec3 viewSpaceNormals = vec3(view * vec4(normals, 0.0));
	vec3 tangent = normalize(randomVec - viewSpaceNormals * dot(randomVec, viewSpaceNormals));
	vec3 bitangent = cross(viewSpaceNormals, tangent);
	mat3 TBN = mat3(tangent, bitangent, viewSpaceNormals);
	
	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i)
	{
		vec3 currentSample = TBN * samples[i];
		currentSample = viewSpacePos + currentSample * radius; 
		vec4 offset = vec4(currentSample, 1.0);
		offset = proj * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;
		float sampleDepth = ViewPosFromNDC(screenUVs, texture(gBuffer.depth, offset.xy).r, invProj).z;
		float bias = 0.25;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewSpacePos.z - sampleDepth));
		occlusion += (sampleDepth >= currentSample.z + bias ? 1.0 : 0.0) * rangeCheck;      
	}
	occlusion = 1.0 - (occlusion / float(kernelSize));
	ssao.r = occlusion;
}