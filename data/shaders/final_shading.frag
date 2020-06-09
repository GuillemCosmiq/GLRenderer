#version 420 core
layout(location = 0) out vec3 sceneHDR;
layout(location = 1) out vec3 bloomHDR;
layout(std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
	mat4 invView;
	mat4 invProj;
};
uniform vec2 viewport;
uniform vec3 camPos;
struct GBuffer
{
	sampler2D albedo;
	highp sampler2D normal;
	sampler2D material;
	highp sampler2D depth;
};
uniform GBuffer gBuffer;
uniform sampler2D HDRsample;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 WorldPosFromNDC(vec2 xy, float depth, mat4 invProj, mat4 invView);
void main()
{
	vec2 screenUVs = gl_FragCoord.xy / viewport;
	float depth = texture(gBuffer.depth, screenUVs).r;
	vec3 material = texture(gBuffer.material, screenUVs).rgb;
	vec3 albedo = texture(gBuffer.albedo, screenUVs).rgb;
	vec3 worldPos = WorldPosFromNDC(screenUVs, depth, invProj, invView);
	vec3 N = texture(gBuffer.normal, screenUVs).rgb;
	vec3 V = normalize(camPos - worldPos);
    vec3 R = reflect(-V, N); 
	
	vec3 amountLight = texture(HDRsample, screenUVs).rgb;

	vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, material.r);
	vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, material.g);  
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - material.r;	  
   
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  material.g * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), material.g)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular) * material.b;

	vec3 PBRresult = ambient + amountLight;
	float brightness = dot(PBRresult, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        bloomHDR.rgb = PBRresult;
    else
        bloomHDR.rgb = vec3(0.0, 0.0, 0.0);
	sceneHDR.rgb = PBRresult;
}