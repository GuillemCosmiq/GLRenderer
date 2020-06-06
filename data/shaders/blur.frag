#version 420 core
layout(location = 0) out vec3 pong;
uniform vec2 viewport;
uniform sampler2D ping;
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
void main()
{             
	vec2 screenUVs = gl_FragCoord.xy / viewport;
    vec2 tex_offset = 1.0 / textureSize(ping, 0);
    vec3 result = texture(ping, screenUVs).rgb * weight[0];
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(ping, screenUVs + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(ping, screenUVs - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(ping, screenUVs + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(ping, screenUVs - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    pong.rgb = result;
}