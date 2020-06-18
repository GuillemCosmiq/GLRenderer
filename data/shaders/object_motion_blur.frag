#version 420 core
layout(location = 0) out vec3 outputSample;

layout(std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
	mat4 invView;
	mat4 invProj;
};

struct GBuffer
{
	sampler2D velocity;
};
uniform GBuffer gBuffer;

uniform vec2 viewport;
uniform sampler2D sceneSample;

const float velocityScale = 60/60;

const int maxSamples = 20;

vec3 WorldPosFromNDC(vec2 xy, float depth, mat4 invProj, mat4 invView);
void main()
{
	  vec2 texelSize = 1.0 / vec2(textureSize(sceneSample, 0));
      vec2 texCoords = gl_FragCoord.xy * texelSize;

      vec2 velocity = texture(gBuffer.velocity, texCoords).rg;
      velocity *= velocityScale;

	  float speed = length(velocity / texelSize);
	  int nSamples = clamp(int(speed), 1, maxSamples);

	  vec3 result = texture(sceneSample, texCoords).rgb;
	  for (int i = 1; i < nSamples; ++i)
	  {
			vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
			result += texture(sceneSample, texCoords + offset).rgb;
	  }

	  outputSample.rgb = result / float(nSamples);
}