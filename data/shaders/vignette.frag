#version 420 core
layout(location = 0) out vec3 outputSample;
uniform vec2 viewport;
uniform sampler2D sceneSample;
uniform float radius;
uniform float softness;
void main()
{
	vec2 texCoords = gl_FragCoord.xy / viewport.xy;
	vec2 texCoordsFromCenter = texCoords - vec2(0.5);
	float len = length(texCoordsFromCenter);
	float vignette = 1.0 - smoothstep(radius-softness, radius, len);
	vec3 sceneColor = texture(sceneSample, texCoords).rgb;
	outputSample = mix(sceneColor, sceneColor *  vignette, 0.5);
}