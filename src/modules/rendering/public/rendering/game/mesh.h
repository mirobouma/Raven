#pragma once
#include "renderer_interface/renderer_interface.h"
#include "rendering/game/material.h"
#include "core/math/transform.h"

namespace Raven
{
	struct FMeshVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 bitangent;
		glm::vec3 tangent;
		glm::vec2 uv;
	};

	struct FSubMesh
	{
		uint32_t mVertexCount;
		uint32_t mVertexOffset;
		uint32_t mIndexCount;
		uint32_t mIndexOffset;
		FMaterial* mMaterial;
		uint32_t   mMaterialIndex;
		FTransform mTransform;
	};

	class RMesh
	{
	public:
		RMesh(IRenderInterface* renderer, std::string file, float importScale);
		~RMesh();

		void Destroy();

		std::vector<FSubMesh> mSubMeshes;
		std::vector<FMaterial> mMaterials;
		FTransform mTransform;
		IRenderInterface* mRenderer;

		IBuffer* mVertexBuffer;
		IBuffer* mIndexBuffer;
	};


}