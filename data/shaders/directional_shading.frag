#version 420 core
layout(location = 0) out vec3 HDRsample;
layout(std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
	mat4 invView;
	mat4 invProj;
};
uniform vec2 viewport;
uniform vec3 camPos;
const float PI = 3.14159265359;
struct GBuffer
{
	sampler2D albedo;
	highp sampler2D normal;
	sampler2D material;
	highp sampler2D depth;
};
uniform GBuffer gBuffer;
uniform sampler2D cumHDRsample;
struct Light
{
	vec3 dir;
	vec3 color;
	int castShadows;
	mat4 lightSpaceMatrix[3];
	sampler2D nearDepth;
	sampler2D midDepth;
	sampler2D farDepth;
	int debugCSM;
	float frustumSplits[3];
};
uniform Light light;
const float csmDebugIntesity = 0.3;


float linearizeDepth(float near, float far, float depth) {
    return 2.0 * near / (far + near - depth * (far - near));
}

vec4 ShadowCalculation(vec3 worldPos, float pixelDepth, vec3 pixelNormal)
{
	int cascadeIndex = 0;
	for (int i = 0; i < 3 - 1; ++i)
	{
		if (linearizeDepth(0.1, 100, pixelDepth) >= light.frustumSplits[i])
			cascadeIndex = i + 1;
	}

	vec4 fragPosLightSpace = light.lightSpaceMatrix[cascadeIndex] * vec4(worldPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	
	vec3 rgbCascade = vec3(0.0, 0.0, 0.0);
	float closestDepth;
	if (cascadeIndex == 0)
	{
		closestDepth = texture(light.nearDepth, projCoords.xy).r;
		rgbCascade.r = csmDebugIntesity;
	}
	else if (cascadeIndex == 1)
	{
		closestDepth = texture(light.midDepth, projCoords.xy).r;
		rgbCascade.g = csmDebugIntesity;	
	}
	else
	{
		closestDepth = texture(light.farDepth, projCoords.xy).r;
		rgbCascade.b = csmDebugIntesity;	
	}
	rgbCascade.rgb *= light.debugCSM;

	float currentDepth = projCoords.z;  
	float bias = max(0.05 * (1.0 - normalize(dot(pixelNormal, -light.dir))), 0.005);	

	return vec4(rgbCascade.rgb, closestDepth < currentDepth ? 1 : 0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 WorldPosFromNDC(vec2 xy, float depth, mat4 invProj, mat4 invView);
void main()
{
	vec2 screenUVs = gl_FragCoord.xy / viewport;
	float depth = texture(gBuffer.depth, screenUVs).r;
	vec3 albedo = texture(gBuffer.albedo, screenUVs).rgb;
	vec3 material = texture(gBuffer.material, screenUVs).rgb;
	vec3 worldPos = WorldPosFromNDC(screenUVs, depth, invProj, invView);

	vec3 direction = -light.dir;
	vec3 N = texture(gBuffer.normal, screenUVs).rgb;		
	vec3 V = normalize(camPos - worldPos);
	vec3 radiance = light.color;
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, material.r);

	vec3 H = normalize(V + direction);
	float NDF = DistributionGGX(N, H, material.g);
	float G = GeometrySmith(N, V, direction, material.g);
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.r;

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, direction), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);

	float NdotL = max(dot(N, direction), 0.0);
	
	vec4 shadow = vec4(0.0, 0.0, 0.0, 0.0);
	if (light.castShadows)
		shadow = ShadowCalculation(worldPos, depth, N);

	HDRsample.rgb += texture(cumHDRsample, screenUVs).rgb + ((kD * albedo / PI + specular) * radiance * NdotL) * (1.0 - shadow.a) + shadow.rgb;
}