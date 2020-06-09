#version 420 core
layout(location = 0) out vec3 pong;
uniform vec2 viewport;
uniform sampler2D ping;
uniform int LOD;
uniform vec2 direction;
uniform float kernel[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
void main()
{             
	vec2 screenUVs = gl_FragCoord.xy / viewport;
    vec2 texelSize = 1.0 / textureSize(ping, LOD);
	texelSize *= direction;
    vec3 result = textureLod(ping, screenUVs, LOD).rgb * kernel[0];
	for(int i = 1; i < 5; ++i)
    {
        result += textureLod(ping, screenUVs + texelSize * i, LOD).rgb * kernel[i];
        result += textureLod(ping, screenUVs - texelSize * i, LOD).rgb * kernel[i];
    }
    pong.rgb = result;
}