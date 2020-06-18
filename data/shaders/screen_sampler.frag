#version 420 core
uniform vec2 viewport;
uniform sampler2D inputSample;
uniform int outputSelection;
out vec4 FragColor;

uniform float nearFrustum = 0.1;
uniform float farFrustum = 100;

float linearizeDepth(float near, float far, float depth) {
    return 2.0 * near / (far + near - depth * (far - near));
}

void main()
{
	vec2 screenUVs = gl_FragCoord.xy / viewport;
	if (outputSelection == 0) // render output
		FragColor = vec4(texture(inputSample, screenUVs).rgb, 1.0);
	else if (outputSelection == 1) // depth
		FragColor = vec4(linearizeDepth(nearFrustum, farFrustum, texture(inputSample, screenUVs).r).xxx, 1);
	else if (outputSelection == 2) // ssao
		FragColor = vec4(texture(inputSample, screenUVs).bbb, 1.0);
	else if (outputSelection == 3) // velocity
		FragColor = vec4(texture(inputSample, screenUVs, 0).rg, 0.0, 1.0);
	else if (outputSelection == 4) // LOD
		FragColor = vec4(textureLod(inputSample, screenUVs, 0).rgb, 1.0);
}