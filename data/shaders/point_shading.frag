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
	sampler2D normal;
	sampler2D material;
	sampler2D depth;
};
uniform GBuffer gBuffer;
uniform sampler2D cumHDRsample;
struct PointLight
{
	vec3 pos;
	vec3 color;
	float radius;
	int castShadows;
	samplerCube depth;
};
uniform PointLight light;

float ShadowCalculation(vec3 fragPos, PointLight lightArg, vec3 dir, vec3 normal)
{
	vec3 fragToLight = fragPos - lightArg.pos; 
    float closestDepth = texture(lightArg.depth, fragToLight).r;
	closestDepth *= lightArg.radius;
	float currentDepth = length(fragToLight);
	float bias = max(0.5 * (1.0 - dot(normal, dir)), 0.005); 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 ComputeRadiance(vec3 lightPos, vec3 lightColor, float lightRadius, vec3 worldPos, vec3 V);
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
	vec3 N = texture(gBuffer.normal, screenUVs).rgb;

	vec3 V = normalize(camPos - worldPos);
	vec3 radiance = ComputeRadiance(light.pos, light.color, light.radius, worldPos, V);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, material.r);
	vec3 L = normalize(light.pos - worldPos);
	vec3 H = normalize(V + L);
	float NDF = DistributionGGX(N, H, material.g);
	float G = GeometrySmith(N, V, L, material.g);
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.r;

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);

	float NdotL = max(dot(N, L), 0.0);

	float shadow = 0.0;
	if (light.castShadows)
		shadow = ShadowCalculation(worldPos, light, L, N);
	else
		shadow = 0.0;
	HDRsample.rgb += texture(cumHDRsample, screenUVs).rgb + ((kD * albedo / PI + specular) * radiance * NdotL) * (1.0 - shadow);
}