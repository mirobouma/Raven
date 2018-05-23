#include "Rendering/RenderGraph.h"
#include "Core/Debug/LogManager.h"
#include <algorithm>

namespace Raven
{
	RRenderGraph::RRenderGraph()
	{

	}

	RRenderGraph::~RRenderGraph()
	{

	}

	void RRenderGraph::execute_graph()
	{

	}

	FRenderGraphResource RRenderPassBuilder::import_resource(std::string name, const FImportResourceDesc& desc)
	{
		// Check if we already have a resource associated with this name.
		auto it = mGraphBuilder->mVirtualResourceMap.find(name);
		if (it != mGraphBuilder->mVirtualResourceMap.end())
		{
			RLOG_WARNING("Tried to import a resource into the render graph builder which already exists in this context.");

			FRenderGraphResource ret;
			ret.mVirtualResource = &it->second;
			ret.name = it->first;
			ret.version = it->second.mMaxVersion;

			return ret;
		}
		else
		{
			// Create a virtual resource description.
			FVirtualResourceDesc resDesc;
			resDesc.mType = desc.mType;

			switch (desc.mType)
			{
			case RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_BUFFER:
			{
				resDesc.mImportedBuffer = desc.pBuffer;
			}break;
			case RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_TEXTURE:
			{
				resDesc.mImportedTexture = desc.pTexture;
			}break;
			case RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_RENDER_TARGET:
			{
				resDesc.mImportedRenderTarget = desc.pRenderTarget;
			}break;
			default:
			{
				RLOG_ERROR("Tried to create a virtual resource for an imported resource, but the resource type does not match any of the imported options.");
			}break;
			}

			// Create virtual resource.
			FVirtualResource& virtualResource = mGraphBuilder->mVirtualResourceMap[name];

			virtualResource.mDesc = resDesc;
			virtualResource.mMaxResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;
			virtualResource.mTotalReadFlags = (FRenderGraphResourceReadFlags)0;
			virtualResource.mTotalWriteFlags = (FRenderGraphResourceWriteFlags)0;
			virtualResource.mMaxVersion = 0;

			FVirtualResourceRef ref;
			ref.mResource = &virtualResource;
			ref.mReadFlags = (FRenderGraphResourceReadFlags)0;
			ref.mWriteFlags = (FRenderGraphResourceWriteFlags)0;
			ref.mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			// Create reference to this resource in the current render pass.
			mPass->mVirtualResourceReferences.push_back(ref);

			// We import a resource, therefore this pass should never be culled.
			mDoNotCull = true;

			// Create resource handle.
			FRenderGraphResource ret;
			ret.mVirtualResource = &virtualResource;
			ret.name = name;
			ret.version = 0;

			return ret;
		}

	}

	FRenderGraphResource RRenderPassBuilder::create_resource(std::string name, const FVirtualResourceDesc& desc)
	{
		// Check if we already have a resource associated with this name.
		auto it = mGraphBuilder->mVirtualResourceMap.find(name);
		if (it != mGraphBuilder->mVirtualResourceMap.end())
		{
			RLOG_WARNING("Tried to create a resource into the render graph builder which already exists in this context.");

			// Create resource handle.
			FRenderGraphResource ret;
			ret.mVirtualResource = &it->second;
			ret.name = it->first;
			ret.version = it->second.mMaxVersion;

			return ret;
		}
		else
		{
			// Create virtual resource.
			FVirtualResource& virtualResource = mGraphBuilder->mVirtualResourceMap[name];

			virtualResource.mDesc = desc;
			virtualResource.mMaxResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;
			virtualResource.mTotalReadFlags = (FRenderGraphResourceReadFlags)0;
			virtualResource.mTotalWriteFlags = (FRenderGraphResourceWriteFlags)0;
			virtualResource.mMaxVersion = 0;

			FVirtualResourceRef ref;
			ref.mResource = &virtualResource;
			ref.mReadFlags = (FRenderGraphResourceReadFlags)0;
			ref.mWriteFlags = (FRenderGraphResourceWriteFlags)0;
			ref.mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			// Create reference to this resource in the current render pass.
			mPass->mVirtualResourceReferences.push_back(ref);

		}

		// Create resource handle.
		FRenderGraphResource ret;
		ret.name = name;
		ret.version = 0;

		return ret;
	}

	FRenderGraphResource RRenderPassBuilder::read_resource(FRenderGraphResource resource, ERenderGraphResourceAccess access, FRenderGraphResourceReadFlags readFlags)
	{
		// Check if we already have a resource associated with this name.
		auto it = mGraphBuilder->mVirtualResourceMap.find(resource.name);
		if (it != mGraphBuilder->mVirtualResourceMap.end())
		{
			FVirtualResource* vResource = &it->second;
			vResource->mTotalReadFlags |= readFlags;

			// We have a higher access requirement for this resource.
			if (access > vResource->mMaxResourceAccess)
			{
				vResource->mMaxResourceAccess = access;
			}

			auto resRef = std::find_if(mPass->mVirtualResourceReferences.begin(), mPass->mVirtualResourceReferences.end(), [&](FVirtualResourceRef& ref) { return ref.mResource == vResource; });
			if (resRef != mPass->mVirtualResourceReferences.end())
			{
				FVirtualResourceRef* ref = (FVirtualResourceRef*)&(*resRef);
				ref->mReadFlags |= readFlags;

				if (access > ref->mResourceAccess)
				{
					ref->mResourceAccess = access;
				}
			}
			else
			{
				FVirtualResourceRef ref;
				ref.mResource = vResource;
				ref.mReadFlags = readFlags;
				ref.mWriteFlags = (FRenderGraphResourceWriteFlags)0;
				ref.mResourceAccess = access;

				mPass->mVirtualResourceReferences.push_back(ref);
			}

		}
		else
		{
			RLOG_ERROR("Tried to read from a resource which does not exist in the render graph.");
		}

		// Create resource handle.
		FRenderGraphResource ret = resource;

		return ret;
	}

	FRenderGraphResource RRenderPassBuilder::write_resource(FRenderGraphResource resource, ERenderGraphResourceAccess access, FRenderGraphResourceWriteFlags writeFlags)
	{
		// Check if we already have a resource associated with this name.
		auto it = mGraphBuilder->mVirtualResourceMap.find(resource.name);
		if (it != mGraphBuilder->mVirtualResourceMap.end())
		{
			FVirtualResource* vResource = &it->second;
			vResource->mTotalWriteFlags |= writeFlags;

			// We have a higher access requirement for this resource.
			if (access > vResource->mMaxResourceAccess)
			{
				vResource->mMaxResourceAccess = access;
			}

			// This is an imported resource we are writing to, so we should never cull this render pass.
			if (vResource->mDesc.mType == RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_BUFFER ||
				vResource->mDesc.mType == RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_TEXTURE ||
				vResource->mDesc.mType == RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_RENDER_TARGET)
			{
				mDoNotCull = true;
			}

			auto resRef = std::find_if(mPass->mVirtualResourceReferences.begin(), mPass->mVirtualResourceReferences.end(), [&](FVirtualResourceRef& ref) { return ref.mResource == vResource; });
			if (resRef != mPass->mVirtualResourceReferences.end())
			{
				FVirtualResourceRef* ref = (FVirtualResourceRef*)&(*resRef);
				ref->mWriteFlags |= writeFlags;

				if (access > ref->mResourceAccess)
				{
					ref->mResourceAccess = access;
				}
			}
			else
			{
				// Create new virtual resource reference
				FVirtualResourceRef ref;
				ref.mResource = vResource;
				ref.mReadFlags = (FRenderGraphResourceReadFlags)0;
				ref.mWriteFlags = writeFlags;
				ref.mResourceAccess = access;

				mPass->mVirtualResourceReferences.push_back(ref);
			}
		}
		else
		{
			RLOG_WARNING("Tried to write to a resource which does not exist in the render graph.");
		}

		// Increment max version for the virtual resource.
		resource.mVirtualResource->mMaxVersion++;

		// Create resource handle.
		FRenderGraphResource ret = resource;
		ret.version = resource.mVirtualResource->mMaxVersion;

		return ret;
	}

	EGraphBuilderResult RRenderGraphBuilder::validate_and_build(RRenderGraph* graph)
	{
		// Vector of passes at the end of a rendering chain.
		std::vector<FBaseRenderGraphPass*> leafPasses;

		for (auto map : mRenderPassMap)
		{
			FBaseRenderGraphPass& pass = map.second;

			// We shouldn't cull the pass, therefore add it to the render graph.
			if (pass.mDoNotCull)
			{
				leafPasses.push_back(&pass);
				continue;
			}
			
			// Loop though all resource references to find out if this is an leaf pass.
			for (FVirtualResourceRef& ref : pass.mVirtualResourceReferences)
			{
				// If this is an imported resource that we write to, we can assume this is a leaf pass.
				if ((ref.mResource->mDesc.mType == RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_BUFFER ||
					ref.mResource->mDesc.mType == RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_TEXTURE ||
					ref.mResource->mDesc.mType == RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_RENDER_TARGET) &&
					ref.mWriteFlags != 0)
				{
					leafPasses.push_back(&pass);
					break;
				}
			}

		}

		return EGraphBuilderResult::GRAPH_BUILDER_RESULT_SUCCESS;
	}

	RRenderGraphBuilder* RRenderGraphManager::get_new_graph_builder()
	{
		return nullptr;
	}
	
}
