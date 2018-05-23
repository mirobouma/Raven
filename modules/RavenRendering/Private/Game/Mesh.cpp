#include "Rendering/Game/Mesh.h"
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
#include "Core/Debug/LogManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Raven
{
	RMesh::RMesh(IRenderer* renderer, std::string path)
	{
		mRenderer = renderer;

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_SplitLargeMeshes |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices);

		auto error = importer.GetErrorString();

		if (scene != NULL)
		{
			int vertexAmount = 0;
			int indexAmount = 0;

			mMaterials.resize(scene->mNumMaterials);
			std::vector<FMeshVertex> vertices;
			std::vector<uint32_t> indices;

			// Load materials
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				aiString path;

				// get color.
				scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, mMaterials[i].mColor);
				if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					int texWidth, texHeight, texChannels;
					stbi_uc* pixels = stbi_load(path.C_Str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
					uint32_t imageSize = texWidth * texHeight * 4;

					if (pixels == nullptr)
					{
						RLOG_ERROR("failed to load texture image: %s!", path.C_Str());
					}
					else
					{
						FTextureCreateDesc texDesc = {};
						texDesc.type = TEXTURE_TYPE_2D;
						texDesc.memoryType = MEMORY_TYPE_GPU;
						texDesc.arrayLayers = 1;
						texDesc.baseArrayLayer = 0;
						texDesc.baseMipLevel = 0;
						texDesc.depth = 1;
						texDesc.format = TEXTURE_FORMAT_RGBA8;
						texDesc.width = texWidth;
						texDesc.height = texHeight;
						texDesc.mipLevels = 1;
						texDesc.pData = pixels;
						texDesc.sampleCount = 1;
						texDesc.sampleQuality = 0;
						texDesc.tiling = IMAGE_TILING_OPTIMAL;
						texDesc.usage = TEXTURE_USAGE_SAMPLED | TEXTURE_USAGE_TRANSFER_DST;

						renderer->createTexture(&mMaterials[i].mTextures[TEXTURE_SLOT_ALBEDO], &texDesc);
					}
				}

				if (scene->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					int texWidth, texHeight, texChannels;
					stbi_uc* pixels = stbi_load(path.C_Str(), &texWidth, &texHeight, &texChannels, STBI_default);
					uint32_t imageSize = texWidth * texHeight * 4;

					if (pixels == nullptr)
					{
						RLOG_ERROR("failed to load texture image: %s!", path.C_Str());
					}
					else
					{
						FTextureCreateDesc texDesc = {};
						texDesc.type = TEXTURE_TYPE_2D;
						texDesc.memoryType = MEMORY_TYPE_GPU;
						texDesc.arrayLayers = 1;
						texDesc.baseArrayLayer = 0;
						texDesc.baseMipLevel = 0;
						texDesc.depth = 1;
						texDesc.format = TEXTURE_FORMAT_RGBA8;
						texDesc.width = texWidth;
						texDesc.height = texHeight;
						texDesc.mipLevels = 1;
						texDesc.pData = pixels;
						texDesc.sampleCount = 1;
						texDesc.sampleQuality = 0;
						texDesc.tiling = IMAGE_TILING_OPTIMAL;
						texDesc.usage = TEXTURE_USAGE_SAMPLED | TEXTURE_USAGE_TRANSFER_DST;

						renderer->createTexture(&mMaterials[i].mTextures[TEXTURE_SLOT_NORMAL], &texDesc);
					}
				}

				if (scene->mMaterials[i]->GetTexture(aiTextureType_SPECULAR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					int texWidth, texHeight, texChannels;
					stbi_uc* pixels = stbi_load(path.C_Str(), &texWidth, &texHeight, &texChannels, STBI_default);
					uint32_t imageSize = texWidth * texHeight * 4;

					if (pixels == nullptr)
					{
						RLOG_ERROR("failed to load texture image: %s!", path.C_Str());
					}
					else
					{
						FTextureCreateDesc texDesc = {};
						texDesc.type = TEXTURE_TYPE_2D;
						texDesc.memoryType = MEMORY_TYPE_GPU;
						texDesc.arrayLayers = 1;
						texDesc.baseArrayLayer = 0;
						texDesc.baseMipLevel = 0;
						texDesc.depth = 1;
						texDesc.format = TEXTURE_FORMAT_RGBA8;
						texDesc.width = texWidth;
						texDesc.height = texHeight;
						texDesc.mipLevels = 1;
						texDesc.pData = pixels;
						texDesc.sampleCount = 1;
						texDesc.sampleQuality = 0;
						texDesc.tiling = IMAGE_TILING_OPTIMAL;
						texDesc.usage = TEXTURE_USAGE_SAMPLED | TEXTURE_USAGE_TRANSFER_DST;

						renderer->createTexture(&mMaterials[i].mTextures[TEXTURE_SLOT_ROUGHNESS], &texDesc);
					}
				}
			}

			// Load vertices, indices, normals and UV co-ordinates.
			for (uint32_t i = 0; i < scene->mNumMeshes; i++)
			{
				int subMeshVertexAmount = scene->mMeshes[i]->mNumVertices;
				int subMeshIndexAmount = scene->mMeshes[i]->mNumFaces * 3;

				for (uint32_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
				{
					aiVector3D vert = scene->mMeshes[i]->mVertices[j];
					aiVector3D normal = scene->mMeshes[i]->mNormals[j];
					aiVector3D tangent = aiVector3D(0.0f, 1.0f, 0.0f);
					aiVector3D bitangent = aiVector3D(0.0f, 1.0f, 0.0f);
					aiVector2D uv = aiVector2D(0.0f, 0.0f);
					
					if (scene->mMeshes[i]->HasTangentsAndBitangents())
					{
						tangent = scene->mMeshes[i]->mTangents[j];
						bitangent = scene->mMeshes[i]->mBitangents[j];
					}

					if (scene->mMeshes[i]->HasTextureCoords(0))
					{
						uv = aiVector2D(scene->mMeshes[i]->mTextureCoords[0][j].x, scene->mMeshes[i]->mTextureCoords[0][j].y);
					}

					FMeshVertex vertex;
					vertex.position = glm::vec3(vert.x, vert.y, vert.z);
					vertex.normal = glm::vec3(normal.x, normal.y, normal.z);
					vertex.uv = glm::vec2(uv.x, uv.y);
					vertex.tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
					vertex.bitangent = glm::vec3(bitangent.x, bitangent.y, bitangent.z);
				}

				for (uint32_t j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					aiFace face = scene->mMeshes[i]->mFaces[j];
					for (uint32_t k = 0; k < face.mNumIndices; k++)
					{
						indices.push_back(face.mIndices[k]);
					}
				}

				FSubMesh mesh;
				mesh.mVertexCount = subMeshVertexAmount;
				mesh.mIndexCount = subMeshIndexAmount;
				mesh.mVertexOffset = vertexAmount;
				mesh.mIndexOffset = indexAmount;
				mesh.mMaterial = &mMaterials[scene->mMeshes[i]->mMaterialIndex];

				vertexAmount += subMeshVertexAmount;
				indexAmount += subMeshIndexAmount;

				mSubMeshes.push_back(mesh);
			}

			// Create index buffer.
			FBufferCreateDesc iBufferDesc = {};
			iBufferDesc.memoryType = MEMORY_TYPE_CPU;
			iBufferDesc.size = indices.size() * sizeof(uint32_t);
			iBufferDesc.usage = BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_INDEX_BUFFER;
			iBufferDesc.data = indices.data();

			renderer->createBuffer(&mIndexBuffer, &iBufferDesc);

			// Create vertex buffer.
			FBufferCreateDesc vBufferDesc = {};
			vBufferDesc.memoryType = MEMORY_TYPE_GPU;
			vBufferDesc.size = vertices.size() * sizeof(FMeshVertex);
			vBufferDesc.usage = BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_VERTEX_BUFFER;
			vBufferDesc.data = vertices.data();

			renderer->createBuffer(&mVertexBuffer, &vBufferDesc);
		}
	}

	void RMesh::Destroy()
	{
		if (mVertexBuffer != nullptr)
		{
			mRenderer->destroyBuffer(mVertexBuffer);
			mVertexBuffer = nullptr;
		}

		if (mIndexBuffer != nullptr)
		{
			mRenderer->destroyBuffer(mIndexBuffer);
			mIndexBuffer = nullptr;
		}
	}

	RMesh::~RMesh()
	{
		Destroy();
	}
}