#version 420 core
layout(location = 0) out vec3 sceneSample;
uniform sampler2D sceneSampleHDR;
uniform sampler2D bloomSampleHDR;
uniform vec2 viewport;
void main()
{
	vec2 screenUVs = gl_FragCoord.xy / viewport;
	vec3 sceneHDR = texture(sceneSampleHDR, screenUVs).rgb;
	vec3 bloomHDR = texture(bloomSampleHDR, screenUVs).rgb;
	sceneSample = sceneHDR + bloomHDR;
}