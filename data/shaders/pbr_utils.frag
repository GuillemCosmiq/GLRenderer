#version 420 core
const float PI = 3.14159265359;
//	float attenuation = (pow(clamp(pow(1.0/lightRadius, 4), 0.0, 1.0), 2) / dist * dist + 1.0) - 4.0;
// https://petapixel.com/2016/06/02/primer-inverse-square-law-light/
// https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
// https://www.desmos.com/calculator/nmnaud1hrw
// https://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
// http://www.ozone3d.net/blogs/lab/20080709/saturate-function-in-glsl/
// http://brabl.com/light-attenuation/
vec3 ComputeRadiance(vec3 lightPos, vec3 lightColor, float lightRadius, vec3 worldPos, vec3 V)
{
	vec3 L = normalize(lightPos - worldPos);
	vec3 H = normalize(V + L);
	float dist = length(lightPos - worldPos);
	float attenuation = clamp(1.0 - dist*dist/(lightRadius*lightRadius), 0.0, 1.0);
	return lightColor * attenuation;
}
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return max(F0 + (1.0 - F0) * pow(1.0 - min(cosTheta, 1.0), 5.0), 0.0);
}
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - min(cosTheta, 1.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}