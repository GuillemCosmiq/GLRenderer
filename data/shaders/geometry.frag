#version 420 core
layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gMaterial;
layout(location = 3) out vec2 gVelocity;

smooth in vec4 out_position;
smooth in vec4 out_prevPosition;
in vec3 outNormal;
in vec2 outTexCoords;
in mat3 outTBN;
struct Material
{
	sampler2D albedoMap;
	sampler2D normalMap;
	sampler2D metallicMap;
	sampler2D roughnessMap;
	int normalsBinded;
};
uniform Material material;

uniform int blurMask;

void main()
{
	if (material.normalsBinded)
	{
		gNormal = texture(material.normalMap, outTexCoords).rgb;
		gNormal = gNormal * 2.0 - 1.0;
		gNormal = normalize(outTBN * gNormal);
	//	gNormal = normalize(outNormal);
	}
	else
	{
		gNormal = normalize(outNormal);
	}
	gAlbedo = vec4(texture(material.albedoMap, outTexCoords).rgb, blurMask);
	gMaterial.r = texture(material.metallicMap, outTexCoords).r;
	gMaterial.g = texture(material.roughnessMap, outTexCoords).r;
	gMaterial.b = 0.0;

	vec2 a = (out_position.xy / out_position.w) * 0.5 + 0.5;
    vec2 b = (out_prevPosition.xy / out_prevPosition.w) * 0.5 + 0.5;
	gVelocity = (a - b);
}