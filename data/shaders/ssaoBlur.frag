#version 420 core
layout(location = 0) out vec3 gMaterial;
  
uniform sampler2D ssaoTexture;

uniform vec2 viewport;

void main() {
	vec2 screenUVs = gl_FragCoord.xy / viewport;
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, screenUVs + offset).r;
        }
    }
    gMaterial.b = result / (4.0 * 4.0);
}  