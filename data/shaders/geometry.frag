#version 420 core
layout(location = 0) out vec3 gAlbedo;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gMaterial;
in vec4 out_position;
in vec4 out_prevPosition;
in vec3 outNormal;
in vec2 outTexCoords;
in mat3 outTBN;
struct Material
{
	sampler2D albedoMap;
	sampler2D normalMap;
	sampler2D metallicMap;
	sampler2D roughnessMap;
	sampler2D aoMap;
	int normalsBinded;
};
uniform Material material;
void main()
{
	if (material.normalsBinded)
	{
		gNormal = normalize(texture(material.normalMap, outTexCoords).rgb);
		gNormal = gNormal * 2.0 - 1.0;
		gNormal = normalize(outTBN * gNormal);
		gNormal = normalize(outNormal);
	}
	else
	{
		gNormal = normalize(outNormal);
	}
	gAlbedo = texture(material.albedoMap, outTexCoords).rgb;
	gMaterial.r = texture(material.metallicMap, outTexCoords).r;
	gMaterial.g = texture(material.roughnessMap, outTexCoords).r;
	gMaterial.b = 1.0;//texture(material.aoMap, outTexCoords).r;
}