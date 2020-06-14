// Copyright (c) 2019 Guillem Costa Miquel, kayter72@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. The above copyright notice and this permission notice shall be included in
//	  all copies or substantial portions of the Software.

#include "model_loader.h"

#include "../engine/entity.h"
#include "../engine/components/transform_component.h"
#include "../engine/components/drawable_component.h"
#include "../engine/systems/resource_system/resource_system.h"
#include "../engine/systems/resource_system/resources/mesh.h"
#include "../engine/systems/resource_system/resources/texture.h"

namespace ModelLoader
{

	constexpr char* AssetsPath = "../data/3d_scenes/";
	static void RecursivePopulateData(const aiScene* pScene, aiNode* node, SceneObjects& scene, GLEngine::ResourceSystem& resSystem);
	static GLEngine::Texture* TextureFromFile(const char* path, const std::string& directory, GLEngine::ResourceSystem& resSystem, bool gamma);

	bool Load(const char* pFIle, SceneObjects& scene, GLEngine::ResourceSystem& resSystem)
	{
		Assimp::Importer importer;

		const aiScene* aiscene = importer.ReadFile(pFIle,
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			//aiProcess_FlipUVs |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (aiscene)
		{
			scene.reserve(aiscene->mNumMeshes);
			for (int i = 0; i < aiscene->mNumMeshes; ++i)
			{
				std::shared_ptr<GLEngine::Entity> entity = std::make_shared<GLEngine::Entity>();
				entity->AddComponent<GLEngine::TransformComponent>();
				entity->AddComponent<GLEngine::DrawableComponent>();
				scene.emplace_back(entity);
			}
			RecursivePopulateData(aiscene, aiscene->mRootNode, scene, resSystem);
		}
		else
		{
			LOG(0, importer.GetErrorString());
			return false;
		}

		return true;
	}

	static void RecursivePopulateData(const aiScene* pScene, aiNode* node, SceneObjects& scene, GLEngine::ResourceSystem& resSystem)
	{
		for (unsigned int i = 0u; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = pScene->mMeshes[node->mMeshes[i]];
			aiMaterial* material = pScene->mMaterials[mesh->mMaterialIndex];

			std::shared_ptr<GLEngine::DrawableComponent> drawableCmp = scene[i]->GetComponent<GLEngine::DrawableComponent>();

			std::vector<GLEngine::Mesh::Vertex> vertices;
			vertices.reserve(mesh->mNumVertices);
			for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
			{
				const aiVector3D* pPos = &(mesh->mVertices[j]);
				const aiVector3D* pNormal = &(mesh->mNormals[j]);
				const aiVector3D* pTangent = &(mesh->mTangents[j]);
				const aiVector3D* pBitangent = &(mesh->mBitangents[j]);
				const aiVector3D* pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &aiVector3D(0.f, 0.f, 0.f);

				GLEngine::Mesh::Vertex v;
				v.m_px = pPos->x;
				v.m_py = pPos->y;
				v.m_pz = pPos->z;

				v.m_nx = pNormal->x;
				v.m_ny = pNormal->y;
				v.m_nz = pNormal->z;

				v.m_tcs = pTexCoord->x;
				v.m_tct = pTexCoord->y;

				v.m_tx = pTangent->x;
				v.m_ty = pTangent->y;
				v.m_tz = pTangent->z;

				v.m_bx = pBitangent->x;
				v.m_by = pBitangent->y;
				v.m_bz = pBitangent->z;

				vertices.emplace_back(std::move(v));
			}

			std::vector<unsigned int> indices;
			indices.reserve(mesh->mNumFaces);
			for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
				const aiFace& Face = mesh->mFaces[j];
				assert(Face.mNumIndices == 3);
				indices.emplace_back(std::move(Face.mIndices[0]));
				indices.emplace_back(std::move(Face.mIndices[1]));
				indices.emplace_back(std::move(Face.mIndices[2]));
			}

			GLEngine::Mesh* meshRef = resSystem.Create<GLEngine::Mesh>();
			meshRef->AttachVertices(std::move(vertices));
			meshRef->AttachIndices(std::move(indices));
			meshRef->SetLayout(GLEngine::Mesh::Layout::indices);
			meshRef->Load();
			drawableCmp->SetMesh(meshRef);

			aiString path;
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					drawableCmp->SetAlbedoMap(TextureFromFile(path.C_Str(), AssetsPath, resSystem, true));
				}
			}
			if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
			{
				if (material->GetTexture(aiTextureType_HEIGHT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					drawableCmp->SetNormalMap(TextureFromFile(path.C_Str(), AssetsPath, resSystem, false));
				}
			}
			if (material->GetTextureCount(aiTextureType_AMBIENT) > 0)
			{
				if (material->GetTexture(aiTextureType_AMBIENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					drawableCmp->SetMetallicMap(TextureFromFile(path.C_Str(), AssetsPath, resSystem, false));
				}
			}
			if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
			{
				if (material->GetTexture(aiTextureType_SHININESS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					drawableCmp->SetRoughnessMap(TextureFromFile(path.C_Str(), AssetsPath, resSystem, false));
				}
			}
			if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
			{
				if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					drawableCmp->SetAOMap(TextureFromFile(path.C_Str(), AssetsPath, resSystem, false));
				}
			}
		}

		for (unsigned int i = 0u; i < node->mNumChildren; ++i)
		{
			RecursivePopulateData(pScene, node->mChildren[i], scene, resSystem);
		}
	}

	GLEngine::Texture* TextureFromFile(const char* path, const std::string& directory, GLEngine::ResourceSystem& resSystem, bool gamma)
	{
		std::string fullPath = directory + path;
		int width, height, nrcomponents;
		unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrcomponents, 0);
		if (data)
		{
			uint32 format;
			uint32 internalFormat;
		
			if (nrcomponents == 1)
			{
				internalFormat = format = GL_RED;
			}
			else if (nrcomponents == 3)
			{
				format = GL_RGB;
				if (gamma)
					internalFormat = GL_SRGB;
				else
					internalFormat = GL_RGB;
			}
			else if (nrcomponents == 4)
			{
				format = GL_RGBA;
				if (gamma)
					internalFormat = GL_SRGB_ALPHA;
				else
					internalFormat = GL_RGBA;
			}
			GLEngine::Texture* texRef = resSystem.Create<GLEngine::Texture>();
			texRef->Create();
			texRef->Bind(0);
			texRef->DefineParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
			texRef->DefineParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
			texRef->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			texRef->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			texRef->DefineBuffer({ width, height }, 0, internalFormat, format, GL_UNSIGNED_BYTE, data);
			texRef->GenerateMipMaps();
			stbi_image_free(data);
			return texRef;
		}
		return nullptr;
	}

}