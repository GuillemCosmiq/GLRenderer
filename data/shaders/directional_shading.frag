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
	sampler2D depth1;
	sampler2D depth2;
	sampler2D depth3;
};
uniform Light light;

float ShadowCalculation(vec4 fragPosLightSpace, Light lightArg, vec3 normal)
{
	//vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//projCoords = projCoords * 0.5 + 0.5;
	//float closestDepth = texture(lightArg.depth, projCoords.xy).r;
	//float currentDepth = projCoords.z;  
	//float bias = max(0.05 * (1.0 - normalize(dot(normal, -lightArg.dir))), 0.005);
	//
	////return closestDepth < currentDepth - bias ? 1 : 0; // in case we want to ignore the box filtering of nearby samples
	//
	//float shadow = 0.0;
	//vec2 texelSize = 1.0 / textureSize(lightArg.depth, 0);
	//for(int x = -1; x <= 1; ++x)
	//{
	//    for(int y = -1; y <= 1; ++y)
	//    {
	//        float pcfDepth = texture(lightArg.depth, projCoords.xy + vec2(x, y) * texelSize).r; 
	//        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	//    }    
	//}
	//shadow /= 9.0;
	//return shadow;
	return 0;
}
float linearizeDepth(float near, float far, float depth) {
    return 2.0 * near / (far + near - depth * (far - near));
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
	
	float cascadeSplits[3] = { 0.4, 0.8, 1.0 };
	int cascadeIndex = 0;
	for (int i = 0; i < 3 - 1; ++i)
	{
		if (linearizeDepth(0.1, 100, depth) >= cascadeSplits[i])
			cascadeIndex = i + 1;
	}

	float shadow = 0.0;
	if (light.castShadows)
	{
	vec4 fragPosLightSpace = light.lightSpaceMatrix[cascadeIndex] * vec4(worldPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth;
	if (cascadeIndex == 0)
		closestDepth = texture(light.depth1, projCoords.xy).r;
		
	if (cascadeIndex == 1)
		closestDepth = texture(light.depth2, projCoords.xy).r;
		
	if (cascadeIndex == 2)
		closestDepth = texture(light.depth3, projCoords.xy).r;
	float currentDepth = projCoords.z;  
	float bias = max(0.05 * (1.0 - normalize(dot(N, -light.dir))), 0.005);

	shadow = closestDepth < currentDepth ? 1 : 0;
	}
		//shadow = ShadowCalculation(light.lightSpaceMatrix * vec4(worldPos, 1.0), light, N);
	else
		shadow = 0.0;

	vec3 rgbCascade = vec3(0.0, 0.0, 0.0);
	if (cascadeIndex == 0)
	rgbCascade.r = 0.3;
	else if (cascadeIndex == 1)
	rgbCascade.g = 0.3;	
	else if (cascadeIndex == 2)
	rgbCascade.b = 0.3;
	//rgbCascade = vec3(0.0, 0.0, 0.0);
	HDRsample.rgb += texture(cumHDRsample, screenUVs).rgb + ((kD * albedo / PI + specular) * radiance * NdotL) * (1.0 - shadow) + rgbCascade;
}