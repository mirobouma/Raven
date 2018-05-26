#include "Rendering/RenderGraph.h"
#include "Core/Debug/LogManager.h"
#include "Core/Utility/Hash.h"
namespace Raven
{
	RRenderGraphManager* GRenderGraphManager = nullptr;

	FVirtualResourceRef* RRenderPassBuilder::import_resource(std::string name, const FImportResourceDesc& desc)
	{
		// Check if we already have a resource associated with this name.
		auto it = mGraphBuilder->mVirtualResources.find(name);
		if (it != mGraphBuilder->mVirtualResources.end())
		{
			RLOG_WARNING("Tried to import a resource into the render graph builder which already exists in this context.");

			FVirtualResourceRef ref = {};
			ref.mName = name;
			ref.mParent = nullptr;
			ref.mResource = &it->second;
			ref.mPassIndex = mPass->mIndex;
			ref.mReadFlags = (FRenderGraphResourceReadFlags)0;
			ref.mWriteFlags = (FRenderGraphResourceWriteFlags)0;
			ref.mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;
		
			mPass->mVirtualResourceReferences.push_back(std::move(ref));

			// Return reference to the newly created resource reference.
			return &mPass->mVirtualResourceReferences[mPass->mVirtualResourceReferences.size() - 1];
		}
		else
		{
			// Create a virtual resource description.
			FVirtualResourceDesc resDesc;
			resDesc.mType = desc.mType;

			switch (desc.mType)
			{
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_BUFFER:
			{
				resDesc.mImportedBuffer = desc.pBuffer;
			}break;
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_TEXTURE:
			{
				resDesc.mImportedTexture = desc.pTexture;
			}break;
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET:
			{
				resDesc.mImportedRenderTarget = desc.pRenderTarget;
			}break;
			default:
			{
				RLOG_ERROR("Tried to create a virtual resource for an imported resource, but the resource type does not match any of the imported options.");
			}break;
			}

			// Create virtual resource.
			FVirtualResource& virtualResource = mGraphBuilder->mVirtualResources[name];

			virtualResource.mName = name;
			virtualResource.mDesc = resDesc;
			virtualResource.mMaxResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;
			virtualResource.mTotalReadFlags = (FRenderGraphResourceReadFlags)0;
			virtualResource.mTotalWriteFlags = (FRenderGraphResourceWriteFlags)0;
			virtualResource.mCreatedInPass = UINT32_MAX;

			// Add reference to this resource to the virtual resource.
			virtualResource.mReferencedInPasses.push_back(mPass->mIndex);

			// Create resource reference.
			FVirtualResourceRef ref = {};
			ref.mName = name;
			ref.mParent = nullptr;
			ref.mPassIndex = mPass->mIndex;
			ref.mResource = &virtualResource;
			ref.mReadFlags = (FRenderGraphResourceReadFlags)0;
			ref.mWriteFlags = (FRenderGraphResourceWriteFlags)0;
			ref.mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			// Create reference to this resource in the current render pass.
			mPass->mVirtualResourceReferences.push_back(std::move(ref));

			// We import a resource, therefore this pass should never be culled.
			setDoNotCull(true);


			FVirtualResourceRef* resRef = &mPass->mVirtualResourceReferences[mPass->mVirtualResourceReferences.size() - 1];

			// Return reference to the newly created resource reference.
			return resRef;
		}

	}

	FVirtualResourceRef* RRenderPassBuilder::create_resource(std::string name, const FVirtualResourceDesc& desc)
	{
		// Check if we already have a resource associated with this name.
		auto it = mGraphBuilder->mVirtualResources.find(name);
		if (it != mGraphBuilder->mVirtualResources.end())
		{
			RLOG_WARNING("Tried to create a resource into the render graph builder which already exists in this context.");

			FVirtualResourceRef ref = {};
			ref.mName = name;
			ref.mParent = nullptr;
			ref.mResource = &it->second;
			ref.mPassIndex = mPass->mIndex;
			ref.mReadFlags = (FRenderGraphResourceReadFlags)0;
			ref.mWriteFlags = (FRenderGraphResourceWriteFlags)0;
			ref.mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			mPass->mVirtualResourceReferences.push_back(std::move(ref));

			// Return reference to the newly created resource reference.
			return &mPass->mVirtualResourceReferences[mPass->mVirtualResourceReferences.size() - 1];
		}
		else
		{
			// Create virtual resource.
			FVirtualResource& virtualResource = mGraphBuilder->mVirtualResources[name];

			virtualResource.mDesc = desc;
			virtualResource.mMaxResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;
			virtualResource.mTotalReadFlags = (FRenderGraphResourceReadFlags)0;
			virtualResource.mTotalWriteFlags = (FRenderGraphResourceWriteFlags)0;
			virtualResource.mCreatedInPass = mPass->mIndex;

			// Add reference to this resource to the virtual resource.
			virtualResource.mReferencedInPasses.push_back(mPass->mIndex);

			// Create resource reference.
			FVirtualResourceRef ref = {};
			ref.mName = name;
			ref.mParent = nullptr;
			ref.mPassIndex = mPass->mIndex;
			ref.mResource = &virtualResource;
			ref.mReadFlags = (FRenderGraphResourceReadFlags)0;
			ref.mWriteFlags = (FRenderGraphResourceWriteFlags)0;
			ref.mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			
			// Create reference to this resource in the current render pass.
			mPass->mVirtualResourceReferences.push_back(std::move(ref));

			// Return reference to the newly created resource reference.
			FVirtualResourceRef* resRef = &mPass->mVirtualResourceReferences[mPass->mVirtualResourceReferences.size() - 1];

			return resRef;	
		}
	}

	FVirtualResourceRef* RRenderPassBuilder::read_resource(std::string name, FVirtualResourceRef* resource, ERenderGraphResourceAccess access, FRenderGraphResourceReadFlags readFlags)
	{
		FVirtualResource* vResource = resource->mResource;
		vResource->mTotalReadFlags |= readFlags;

		// We have a higher access requirement for this resource.
		if (access > vResource->mMaxResourceAccess)
		{
			vResource->mMaxResourceAccess = access;
		}

		// Check if we already reference this resource in this pass.
		auto resRef = std::find_if(mPass->mVirtualResourceReferences.begin(), mPass->mVirtualResourceReferences.end(), 
			[&](FVirtualResourceRef& ref) { 
			return ref.mResource == vResource; 
		});

		// If we already reference this resource, update the reference with additional access flags.
		if (resRef != mPass->mVirtualResourceReferences.end())
		{
			FVirtualResourceRef* ref = (FVirtualResourceRef*)&(*resRef);
			ref->mReadFlags |= readFlags;

			if (access > ref->mResourceAccess)
			{
				ref->mResourceAccess = access;
			}

			return ref;
		}
		else
		{
			// Create new virtual resource reference
			mPass->mVirtualResourceReferences.push_back(FVirtualResourceRef());
			FVirtualResourceRef* ref = &mPass->mVirtualResourceReferences[mPass->mVirtualResourceReferences.size() - 1];

			ref->mResource = resource->mResource;
			ref->mName = name;
			ref->mParent = resource;
			ref->mPassIndex = mPass->mIndex;
			ref->mReadFlags = readFlags;
			ref->mWriteFlags = (FRenderGraphResourceWriteFlags)0;
			ref->mResourceAccess = access;

			// Add child to the parent resource.
			resource->mChildren.push_back(ref);
			mPass->mInputs.push_back(ref);

			return ref;
		}	
	}

	FVirtualPass* RRenderGraphBuilder::add_pass(std::string name, std::function<void(RRenderPassBuilder&)> setupFunc)
	{
		FVirtualPass pass(name, setupFunc);
		RRenderPassBuilder builder(this, &pass);

		// Setup this pass.
		pass.mSetupFunc(builder);

		// Set pass index.
		uint32_t index = static_cast<uint32_t>(mRenderPasses.size());
		pass.mIndex = index;

		// Add this virtual pass to the render passes array.
		mRenderPasses.push_back(std::move(pass));

		return &mRenderPasses[index];
	}

	FVirtualResourceRef* RRenderPassBuilder::write_resource(std::string name, FVirtualResourceRef* resource, ERenderGraphResourceAccess access, FRenderGraphResourceWriteFlags writeFlags)
	{
		FVirtualResource* vResource = resource->mResource;
		vResource->mTotalWriteFlags |= writeFlags;

		// We have a higher access requirement for this resource.
		if (access > vResource->mMaxResourceAccess)
		{
			vResource->mMaxResourceAccess = access;
		}

		// If this is an imported resource we are writing to, we should never cull this render pass.
		if (vResource->mDesc.mType == VIRTUAL_RESOURCE_TYPE_IMPORTED_BUFFER ||
			vResource->mDesc.mType == VIRTUAL_RESOURCE_TYPE_IMPORTED_TEXTURE ||
			vResource->mDesc.mType == VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET)
		{
			setDoNotCull(true);
		}

		auto resRef = std::find_if(mPass->mVirtualResourceReferences.begin(), mPass->mVirtualResourceReferences.end(),
			[&](FVirtualResourceRef& ref) { 
			return ref.mResource == vResource; 
		});

		if (resRef != mPass->mVirtualResourceReferences.end())
		{
			// If we already have a resource reference, update its access flags.
			FVirtualResourceRef* ref = (FVirtualResourceRef*)&(*resRef);
			ref->mWriteFlags |= writeFlags;

			if (access > ref->mResourceAccess)
			{
				ref->mResourceAccess = access;
			}

			return ref;
		}
		else
		{
			// Create new virtual resource reference
			mPass->mVirtualResourceReferences.push_back(FVirtualResourceRef());
			FVirtualResourceRef* ref = &mPass->mVirtualResourceReferences[mPass->mVirtualResourceReferences.size() - 1];

			ref->mResource = resource->mResource;
			ref->mName = name;
			ref->mParent = resource;
			ref->mPassIndex = mPass->mIndex;
			ref->mReadFlags = (FRenderGraphResourceReadFlags)0;
			ref->mWriteFlags = writeFlags;
			ref->mResourceAccess = access;

			// Add child to the parent resource.
			resource->mChildren.push_back(ref);
			mPass->mOutputs.push_back(ref);

			return ref;
		}
	}

	EGraphBuilderResult RRenderGraphBuilder::validate_and_build(RRenderGraph** graph)
	{
		RRenderGraph* retGraph = new RRenderGraph();

		// Validate that all the passes are compatible.
		validate_passes();
		
		// calculate dependencies for all passes.
		std::map<uint32_t, std::vector<uint32_t>> dependencyGraph;
		calculate_dependencies(dependencyGraph);

		*graph = retGraph;

		return EGraphBuilderResult::GRAPH_BUILDER_RESULT_SUCCESS;
	}

	EGraphBuilderResult RRenderGraphBuilder::validate_passes()
	{
		for (FVirtualPass& pass : mRenderPasses)
		{
			for (FVirtualResourceRef& resource : pass.mVirtualResourceReferences)
			{
				if (((resource.mReadFlags & RENDER_GRAPH_RESOURCE_READ_FLAG_DEPTH_READ) != 0) && ((resource.mWriteFlags & RENDER_GRAPH_RESOURCE_WRITE_FLAG_DEPTH_WRITE) != 0))
				{
					RLOG_WARNING("Trying to both read and write to a depth-stencil resource in a pass.");
				}

				if (((resource.mReadFlags & RENDER_GRAPH_RESOURCE_READ_FLAG_SAMPLED) != 0) && ((resource.mWriteFlags & RENDER_GRAPH_RESOURCE_WRITE_FLAG_COLOR_ATTACHMENT) != 0))
				{
					RLOG_WARNING("Trying to sample a resource which is also being written to as color attachment.");
				}

				if (((resource.mReadFlags & RENDER_GRAPH_RESOURCE_READ_FLAG_SAMPLED) != 0) && ((resource.mWriteFlags & RENDER_GRAPH_RESOURCE_WRITE_FLAG_DEPTH_WRITE) != 0))
				{
					RLOG_WARNING("Trying to sample a resource which is also being written to as depth attachment.");
				}

				if (((resource.mReadFlags & RENDER_GRAPH_RESOURCE_READ_FLAG_SAMPLED) != 0) && ((resource.mWriteFlags & RENDER_GRAPH_RESOURCE_WRITE_FLAG_UAV) != 0))
				{
					RLOG_WARNING("Trying to sample a resource which is also being written to as an UAV attachment.");
				}
			}

			if (pass.mSetupFunc == nullptr && pass.mExecuteFunc == nullptr)
			{
				RLOG_WARNING("Render pass: \"%s\" has neither an setup or execute function, therefore you should consider removing this pass.", pass.mName);
			}
		}

		return EGraphBuilderResult::GRAPH_BUILDER_RESULT_SUCCESS;
	}

	void RRenderGraphBuilder::calculate_dependencies(std::map<uint32_t, std::vector<uint32_t>>& dependencyMap)
	{
		// Calculate dependencies for each render pass.
		for (uint32_t i = 0; i < mRenderPasses.size(); ++i)
		{
			for (FVirtualResourceRef& ref : mRenderPasses[i].mVirtualResourceReferences)
			{
				uint32_t passIndex = getParentDependency(&ref);

				if (passIndex != UINT32_MAX)
				{
					dependencyMap[i].push_back(passIndex);
				}		
			}
		}
	}

	uint32_t RRenderGraphBuilder::getParentDependency(FVirtualResourceRef* resource)
	{
		if (resource->mParent == nullptr)
		{
			return UINT32_MAX;
		}

		if (resource->mParent->mPassIndex != UINT32_MAX)
		{
			if (resource->mParent->mWriteFlags != 0)
			{
				return resource->mParent->mPassIndex;
			}
			else
			{
				return getParentDependency(resource->mParent);
			}
		}
		else
		{
			return UINT32_MAX;
		}
	}

	RRenderGraphBuilder* RRenderGraphManager::get_new_graph_builder()
	{
		return nullptr;
	}

	void RRenderGraph::execute()
	{

	}
	
}
