#version 420 core
layout(location = 0) out vec3 outputSample;
uniform sampler2D sceneSampleHDR;
uniform sampler2D filteredBloomSampleHDR;
uniform vec2 viewport;
uniform float gamma;
uniform float exposure;
uniform bool IsBloomActive;
uniform bool IsToneMappingActive;
uniform bool IsGammaCorrectionActive;

void main()
{
	vec2 screenUVs = gl_FragCoord.xy / viewport;
	vec3 sceneHDR = texture(sceneSampleHDR, screenUVs).rgb;
	vec3 bloomHDR = texture(filteredBloomSampleHDR, screenUVs).rgb;
	vec3 result = sceneHDR + bloomHDR;
	if (IsToneMappingActive)
		result = vec3(1.0) - exp(-result * exposure);	
	if (IsGammaCorrectionActive)
		result = pow(result, vec3(1.0 / gamma));
	outputSample = result;
}