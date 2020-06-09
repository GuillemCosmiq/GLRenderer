#version 420 core
layout(location = 0) out vec3 outputSample;
uniform sampler2D sceneSampleHDR;
uniform sampler2D filteredBloomSampleHDR;
uniform vec2 viewport;
uniform float gamma;
uniform float exposure;
uniform float BloomLODIntesities[5];
uniform bool IsBloomActive;
uniform bool IsToneMappingActive;
uniform bool IsGammaCorrectionActive;

void main()
{
	vec2 screenUVs = gl_FragCoord.xy / viewport;
	vec3 outputColor = texture(sceneSampleHDR, screenUVs).rgb;

	if (IsBloomActive)
	{
		for (int LOD = 0; LOD < 5; ++LOD)
			outputColor += textureLod(filteredBloomSampleHDR, screenUVs, LOD).rgb * BloomLODIntesities[LOD];
	}

	if (IsToneMappingActive)
	{
		outputColor = vec3(1.0) - exp(-outputColor * exposure);
	}

	if (IsGammaCorrectionActive)
	{
		outputColor = pow(outputColor, vec3(1.0 / gamma));
	}

	outputSample = outputColor;
}