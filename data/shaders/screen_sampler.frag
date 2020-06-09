#version 420 core
uniform vec2 viewport;
uniform sampler2D sceneSample;
out vec4 FragColor;
void main()
{
	vec2 screenUVs = gl_FragCoord.xy / viewport;
	FragColor = vec4(textureLod(sceneSample, screenUVs, 0).rgb, 1.0);
}