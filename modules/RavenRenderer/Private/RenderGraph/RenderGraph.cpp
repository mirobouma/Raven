#include "Renderer/RenderGraph/RenderGraph.h"
#include "Core/Debug/LogManager.h"
#include "Core/Utility/Hash.h"
#include <algorithm>

namespace Raven
{
	FResourceState gReadFlagTranslator(FRenderGraphResourceReadState state)
	{
		FResourceState ret = FResourceState::RESOURCE_STATE_UNDEFINED;

		switch (state)
		{
		case FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED:
		{
			ret = RESOURCE_STATE_SHADER_RESOURCE;
		}
		break;
		case FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_DEPTH_READ:
		{
			ret = RESOURCE_STATE_DEPTH_READ;
		}
		break;
		case FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_PRESENT:
		{
			ret = RESOURCE_STATE_PRESENT;
		}
		break;
		}

		return ret;
	};

	constexpr EMemoryType gResourceAccessToMemoryTypeTranslator(ERenderGraphResourceAccess access)
	{
		EMemoryType type = EMemoryType::MEMORY_TYPE_GPU;

		switch (access)
		{
		case ERenderGraphResourceAccess::RENDER_GRAPH_RESOURCE_ACCESS_GPU:
		{
			type = EMemoryType::MEMORY_TYPE_GPU;
		}
		break;
		case ERenderGraphResourceAccess::RENDER_GRAPH_RESOURCE_ACCESS_CPU:
		{
			type = EMemoryType::MEMORY_TYPE_CPU_TO_GPU;
		}
		break;
		case ERenderGraphResourceAccess::RENDER_GRAPH_RESOURCE_ACCESS_GPU_AND_CPU:
		{
			type = EMemoryType::MEMORY_TYPE_GPU_TO_CPU;
		}
		break;
		default:
		{
			RLOG_ERROR("Unknown render graph resource access, will default memory type to GPU.");
		}
		break;
		}

		return type;
	}

	FImageUsageFlags virtualResourceFlagsToTextureUsage(FRenderGraphResourceReadState read, FRenderGraphResourceWriteState write)
	{
		FImageUsageFlags flags = FImageUsageFlags::IMAGE_USAGE_SAMPLED;

		if ((read & FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED) != 0)
		{
			flags |= FImageUsageFlags::IMAGE_USAGE_SAMPLED;
		}

		if ((read & FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_DEPTH_READ) != 0)
		{
			flags |= FImageUsageFlags::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT;
		}

		// Created resources should never be able to be presented since backbuffers are imported targets, however, add this for security.
		if ((read & FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_PRESENT) != 0)
		{
			flags |= FImageUsageFlags::IMAGE_USAGE_COLOR_ATTACHMENT;
		}

		if ((read & FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_READBACK) != 0)
		{
			flags |= FImageUsageFlags::IMAGE_USAGE_TRANSFER_SRC;
		}

		if ((write & FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_UAV) != 0)
		{
			flags |= FImageUsageFlags::IMAGE_USAGE_STORAGE;
		}

		if ((write & FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT) != 0)
		{
			flags |= FImageUsageFlags::IMAGE_USAGE_COLOR_ATTACHMENT;
		}

		if ((write & FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE) != 0)
		{
			flags |= FImageUsageFlags::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT;
		}

		return flags;
	}

	FBufferUsageFlags virtualResourceFlagsToBufferUsage(FRenderGraphResourceReadState read, FRenderGraphResourceWriteState write)
	{
		FBufferUsageFlags flags = (FBufferUsageFlags)0;

		if ((read & FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED) != 0)
		{
			flags |= FBufferUsageFlags::BUFFER_USAGE_UNIFORM_BUFFER;
		}

		if ((read & FRenderGraphResourceReadState::RENDER_GRAPH_RESOURCE_READ_STATE_READBACK) != 0)
		{
			flags |= FBufferUsageFlags::BUFFER_USAGE_TRANSFER_SRC;
		}

		if ((write & FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_UAV) != 0)
		{
			flags |= FBufferUsageFlags::BUFFER_USAGE_STORAGE_BUFFER;
		}

		return flags;
	}



	FResourceState gWriteFlagTranslator(FRenderGraphResourceWriteState state)
	{
		FResourceState ret = FResourceState::RESOURCE_STATE_UNDEFINED;

		switch (state)
		{
		case FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT:
		{
			ret = RESOURCE_STATE_RENDER_TARGET;
		}
		break;
		case FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE:
		{
			ret = RESOURCE_STATE_DEPTH_WRITE;
		}
		break;
		case FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_UAV:
		{
			ret = RESOURCE_STATE_UNORDERED_ACCESS;
		}
		break;
		default:
		{
			RLOG_WARNING("Unknown write flags when trying to translate FRenderGraphResourceWriteState to FResourceState");
		}
		}

		return ret;
	};

	FVirtualResourceRef* RRenderPassBuilder::import_resource(std::string name, const FImportResourceDesc& desc)
	{
		// Check if we already have a resource associated with this name.
		auto it = mGraphBuilder->mVirtualResources.find(name);
		if (it != mGraphBuilder->mVirtualResources.end())
		{
			RLOG_WARNING("Tried to import a resource into the render graph builder which already exists in this context.");

			FVirtualResourceRef* ref = new FVirtualResourceRef();
			ref->mName = name;
			ref->mParent = nullptr;
			ref->mResource = it->second.get();
			ref->mPassIndex = mPass->mIndex;
			ref->mReadFlags = (FRenderGraphResourceReadState)0;
			ref->mWriteFlags = (FRenderGraphResourceWriteState)0;
			ref->mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			mPass->mVirtualResourceReferences.push_back(std::unique_ptr<FVirtualResourceRef>(ref));

			return ref;
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
			mGraphBuilder->mVirtualResources[name] = std::unique_ptr<FVirtualResource>(new FVirtualResource());
			FVirtualResource* virtualResource = mGraphBuilder->mVirtualResources[name].get();

			virtualResource->mName = name;
			virtualResource->mDesc = resDesc;
			virtualResource->mMaxResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;
			virtualResource->mTotalReadFlags = (FRenderGraphResourceReadState)0;
			virtualResource->mTotalWriteFlags = (FRenderGraphResourceWriteState)0;
			virtualResource->mCreatedInPass = UINT32_MAX;

			// Add reference to this resource to the virtual resource.
			virtualResource->mReferencedInPasses.push_back(mPass->mIndex);

			// Create resource reference.
			FVirtualResourceRef* ref = new FVirtualResourceRef();
			ref->mName = name;
			ref->mParent = nullptr;
			ref->mPassIndex = mPass->mIndex;
			ref->mResource = virtualResource;
			ref->mReadFlags = (FRenderGraphResourceReadState)0;
			ref->mWriteFlags = (FRenderGraphResourceWriteState)0;
			ref->mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			// Create reference to this resource in the current render pass.
			mPass->mVirtualResourceReferences.push_back(std::unique_ptr<FVirtualResourceRef>(ref));

			// We import a resource, therefore this pass should never be culled.
			setDoNotCull(true);

			return ref;
		}

	}

	FVirtualResourceRef* RRenderPassBuilder::create_resource(std::string name, const FVirtualResourceDesc& desc)
	{
		// Check if we already have a resource associated with this name.
		auto it = mGraphBuilder->mVirtualResources.find(name);
		if (it != mGraphBuilder->mVirtualResources.end())
		{
			RLOG_WARNING("Tried to create a resource into the render graph builder which already exists in this context.");

			FVirtualResourceRef* ref = new FVirtualResourceRef();

			ref->mName = name;
			ref->mParent = nullptr;
			ref->mResource = it->second.get();
			ref->mPassIndex = mPass->mIndex;
			ref->mReadFlags = (FRenderGraphResourceReadState)0;
			ref->mWriteFlags = (FRenderGraphResourceWriteState)0;
			ref->mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			// Create reference to this resource in the current render pass.
			mPass->mVirtualResourceReferences.push_back(std::unique_ptr<FVirtualResourceRef>(ref));

			// Return reference to the newly created resource reference.
			return ref;
		}
		else
		{
			// Create virtual resource.
			mGraphBuilder->mVirtualResources[name] = std::unique_ptr<FVirtualResource>(new FVirtualResource());
			FVirtualResource* virtualResource = mGraphBuilder->mVirtualResources[name].get();

			virtualResource->mName = name;
			virtualResource->mDesc = desc;
			virtualResource->mMaxResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;
			virtualResource->mTotalReadFlags = (FRenderGraphResourceReadState)0;
			virtualResource->mTotalWriteFlags = (FRenderGraphResourceWriteState)0;
			virtualResource->mCreatedInPass = mPass->mIndex;

			// Add reference to this resource to the virtual resource.
			virtualResource->mReferencedInPasses.push_back(mPass->mIndex);

			FVirtualResourceRef* ref = new FVirtualResourceRef();

			// Create resource reference.
			ref->mName = name;
			ref->mParent = nullptr;
			ref->mPassIndex = mPass->mIndex;
			ref->mResource = virtualResource;
			ref->mReadFlags = (FRenderGraphResourceReadState)0;
			ref->mWriteFlags = (FRenderGraphResourceWriteState)0;
			ref->mResourceAccess = RENDER_GRAPH_RESOURCE_ACCESS_GPU;

			// Create reference to this resource in the current render pass.
			mPass->mVirtualResourceReferences.push_back(std::unique_ptr<FVirtualResourceRef>(ref));

			return ref;
		}
	}

	FVirtualResourceRef* RRenderPassBuilder::read_resource(std::string name, FVirtualResourceRef* resource, ERenderGraphResourceAccess access, FRenderGraphResourceReadState readFlags)
	{
		if (resource == nullptr)
		{
			return nullptr;
		}

		FVirtualResource* vResource = resource->mResource;
		vResource->mTotalReadFlags |= readFlags;

		// We have a higher access requirement for this resource.
		if (access > vResource->mMaxResourceAccess)
		{
			vResource->mMaxResourceAccess = access;
		}

		// Check if we already reference this resource in this pass.
		auto resRef = std::find_if(mPass->mVirtualResourceReferences.begin(), mPass->mVirtualResourceReferences.end(),
			[&](std::unique_ptr<FVirtualResourceRef>& ref) {
			return ref.get()->mResource == vResource;
		});

		// If we already reference this resource, update the reference with additional access flags.
		if (resRef != mPass->mVirtualResourceReferences.end())
		{
			FVirtualResourceRef* ref = (FVirtualResourceRef*)&(*resRef);
			ref->mReadFlags = readFlags;

			if (access > ref->mResourceAccess)
			{
				ref->mResourceAccess = access;
			}

			return ref;
		}
		else
		{
			// Create new virtual resource reference
			FVirtualResourceRef* ref = new FVirtualResourceRef();

			ref->mResource = resource->mResource;
			ref->mName = name;
			ref->mParent = resource;
			ref->mPassIndex = mPass->mIndex;
			ref->mReadFlags = readFlags;
			ref->mWriteFlags = (FRenderGraphResourceWriteState)0;
			ref->mResourceAccess = access;

			mPass->mVirtualResourceReferences.push_back(std::unique_ptr<FVirtualResourceRef>(ref));

			// Add child to the parent resource.
			resource->mChildren.push_back(ref);
			mPass->mInputs.push_back(ref);

			return ref;
		}
	}

	RRenderGraphBuilder::~RRenderGraphBuilder()
	{
		mRenderPasses.clear();
		mVirtualResources.clear();
	}

	FVirtualTextureDesc FVirtualResourceDesc::GetTextureDesc()
	{
		switch (mType)
		{
		case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET:
		{
			const FImageCreateDesc* importedTextureDesc = mImportedRenderTarget->GetImage()->GetDesc();

			FVirtualTextureDesc desc = {};
			desc.depth = importedTextureDesc->depth;
			desc.width = importedTextureDesc->width;
			desc.height = importedTextureDesc->height;
			desc.sampleCount = importedTextureDesc->sampleCount;
			desc.sampleQuality = importedTextureDesc->sampleQuality;
			desc.format = importedTextureDesc->format;

			return desc;
		}
		break;
		case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_IMPORTED_TEXTURE:
		{
			const FImageCreateDesc* importedTextureDesc = mImportedTexture->GetDesc();

			FVirtualTextureDesc desc = {};
			desc.depth = importedTextureDesc->depth;
			desc.width = importedTextureDesc->width;
			desc.height = importedTextureDesc->height;
			desc.sampleCount = importedTextureDesc->sampleCount;
			desc.sampleQuality = importedTextureDesc->sampleQuality;
			desc.format = importedTextureDesc->format;

			return desc;
		}
		break;
		case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_TEXTURE:
		{
			return mTextureDesc;
		}
		break;
		case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_BUFFER:
		{
			FVirtualTextureDesc desc = {};
			desc.width = mBufferDesc.size;
		}
		break;
		case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_IMPORTED_BUFFER:
		{
			FVirtualTextureDesc desc = {};
			desc.width = mImportedBuffer->getSize();
		}
		break;
		}

		return FVirtualTextureDesc();
	}

	FVirtualPass* RRenderGraphBuilder::add_pass(std::string name, std::function<void(RRenderPassBuilder&)> setupFunc)
	{
		// Set pass index.
		uint32_t index = static_cast<uint32_t>(mRenderPasses.size());

		// Add this virtual pass to the render passes array.
		mRenderPasses.push_back(std::unique_ptr<FVirtualPass>(new FVirtualPass(name, setupFunc)));

		// Add pass.
		FVirtualPass* pass = mRenderPasses[index].get();
		pass->mIndex = index;

		RRenderPassBuilder builder(this, pass);

		// Setup this pass.
		pass->mSetupFunc(builder);

		return pass;
	}

	FVirtualResourceRef* RRenderPassBuilder::write_resource(std::string name, FVirtualResourceRef* resource, ERenderGraphResourceAccess access, FRenderGraphResourceWriteState writeFlags)
	{
		if (resource == nullptr)
		{
			return nullptr;
		}

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
			[&](std::unique_ptr<FVirtualResourceRef>& ref) {
			return ref.get()->mResource == vResource;
		});

		if (resRef != mPass->mVirtualResourceReferences.end())
		{
			// If we already have a resource reference, update its access flags.
			FVirtualResourceRef* ref = (FVirtualResourceRef*)&(*resRef);
			ref->mWriteFlags = writeFlags;

			if (access > ref->mResourceAccess)
			{
				ref->mResourceAccess = access;
			}

			return ref;
		}
		else
		{
			// Create new virtual resource reference
			FVirtualResourceRef* ref = new FVirtualResourceRef();

			ref->mResource = resource->mResource;
			ref->mName = name;
			ref->mParent = resource;
			ref->mPassIndex = mPass->mIndex;
			ref->mReadFlags = (FRenderGraphResourceReadState)0;
			ref->mWriteFlags = writeFlags;
			ref->mResourceAccess = access;

			mPass->mVirtualResourceReferences.push_back(std::unique_ptr<FVirtualResourceRef>(ref));

			// Add child to the parent resource.
			resource->mChildren.push_back(ref);
			mPass->mOutputs.push_back(ref);

			return ref;
		}
	}


	void RRenderGraphBuilder::createPhysicalResources(RRenderGraph* graph)
	{
		std::vector<RPhysicalResource*> mTakenResources;
		for (auto resourcePair = mVirtualResources.begin(); resourcePair != mVirtualResources.end(); ++resourcePair)
		{
			FVirtualResource* resource = resourcePair->second.get();

			switch (resource->mDesc.mType)
			{
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_BUFFER:
			{
				RPhysicalResource* physRes = new RPhysicalResource();

				physRes->mBuffer = resource->mDesc.mImportedBuffer;
				physRes->mType = PHYSICAL_RESOURCE_TYPE_BUFFER;
				physRes->mDesc = resource->mDesc;

				mManager->mResources.insert(std::make_pair(std::unique_ptr<RPhysicalResource>(physRes), mManager->mFrameCounter));
				graph->mPhysicalResources[resource->mName] = physRes;
			}
			break;
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET:
			{
				RPhysicalResource* physRes = new RPhysicalResource();

				physRes->mRenderTarget = resource->mDesc.mImportedRenderTarget;
				physRes->mType = PHYSICAL_RESOURCE_TYPE_RENDER_TARGET;
				physRes->mDesc = resource->mDesc;

				mManager->mResources.insert(std::make_pair(std::unique_ptr<RPhysicalResource>(physRes), mManager->mFrameCounter));
				graph->mPhysicalResources[resource->mName] = physRes;
			}
			break;
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_TEXTURE:
			{
				RPhysicalResource* physRes = new RPhysicalResource();

				physRes->mImage = resource->mDesc.mImportedTexture;
				physRes->mType = PHYSICAL_RESOURCE_TYPE_TEXTURE;
				physRes->mDesc = resource->mDesc;

				mManager->mResources.insert(std::make_pair(std::unique_ptr<RPhysicalResource>(physRes), mManager->mFrameCounter));
				graph->mPhysicalResources[resource->mName] = physRes;
			}
			break;
			case VIRTUAL_RESOURCE_TYPE_BUFFER:
			{
				bool foundCompatibleResource = false;

				for (auto it = mManager->mResources.begin(); it != mManager->mResources.end(); ++it)
				{
					// Check if this virtual resource is compatible.
					if (isVirtualResourceCompatible(resource, it->first.get()))
					{
						auto physResource = std::find(mTakenResources.begin(), mTakenResources.end(), it->first.get());

						// Check if this physical resource is not already used by a different virtual resource.
						if (physResource == mTakenResources.end())
						{
							mTakenResources.push_back(it->first.get());
							it->second = mManager->mFrameCounter;

							graph->mPhysicalResources[resource->mName] = it->first.get();
							foundCompatibleResource = true;
							break;
						}
					}
				}

				// We didn't find a compatible resource therefore we will have to create a new one.
				if (!foundCompatibleResource)
				{
					RPhysicalResource* physRes = new RPhysicalResource();

					physRes->mType = EPhysicalResourceType::PHYSICAL_RESOURCE_TYPE_BUFFER;

					FBufferCreateDesc desc;
					desc.pData = nullptr;
					desc.size = resource->mDesc.mBufferDesc.size;
					desc.memoryType = gResourceAccessToMemoryTypeTranslator(resource->mMaxResourceAccess);
					desc.usage = virtualResourceFlagsToBufferUsage(resource->mTotalReadFlags, resource->mTotalWriteFlags);

					mManager->GetRenderer()->createBuffer(&physRes->mBuffer, &desc);
					physRes->mDesc = resource->mDesc;

					// Register the physical resource.
					mTakenResources.push_back(physRes);
					mManager->mResources.insert(std::make_pair(std::unique_ptr<RPhysicalResource>(physRes), mManager->mFrameCounter));
					graph->mPhysicalResources[resource->mName] = physRes;
				}
			}
			break;
			case VIRTUAL_RESOURCE_TYPE_TEXTURE:
			{
				bool foundCompatibleResource = false;

				for (auto it = mManager->mResources.begin(); it != mManager->mResources.end(); ++it)
				{
					// Check if this virtual resource is compatible.
					if (isVirtualResourceCompatible(resource, it->first.get()))
					{
						auto physResource = std::find(mTakenResources.begin(), mTakenResources.end(), it->first.get());

						// Check if this physical resource is not already used by a different virtual resource.
						if (physResource == mTakenResources.end())
						{
							mTakenResources.push_back(it->first.get());
							it->second = mManager->mFrameCounter;

							graph->mPhysicalResources[resource->mName] = it->first.get();
							foundCompatibleResource = true;
							break;
						}
					}
				}

				// We didn't find a compatible resource therefore we will have to create a new one.
				if (!foundCompatibleResource)
				{
					RPhysicalResource* physRes = new RPhysicalResource();

					// Check if this resource should be a render target.
					if ((resource->mTotalWriteFlags & FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT) != 0 ||
						(resource->mTotalWriteFlags & FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE) != 0)
					{
						physRes->mType = EPhysicalResourceType::PHYSICAL_RESOURCE_TYPE_RENDER_TARGET;

						FRenderTargetCreateDesc desc;
						desc.format = resource->mDesc.mTextureDesc.format;
						desc.width = resource->mDesc.mTextureDesc.width;
						desc.height = resource->mDesc.mTextureDesc.height;
						desc.nativeSwapchainImage = nullptr;
						desc.sampleCount = resource->mDesc.mTextureDesc.sampleCount;
						desc.sampleQuality = resource->mDesc.mTextureDesc.sampleQuality;
						desc.memoryType = gResourceAccessToMemoryTypeTranslator(resource->mMaxResourceAccess);

						desc.usage = ((resource->mTotalWriteFlags & FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT) != 0
							? FRenderTargetUsage::RENDER_TARGET_USAGE_COLOR_ATTACHMENT : (FRenderTargetUsage)0) |
							((resource->mTotalWriteFlags & FRenderGraphResourceWriteState::RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE) != 0
								? FRenderTargetUsage::RENDER_TARGET_USAGE_DEPTH_STENCIL_ATTACHMENT : (FRenderTargetUsage)0);

						desc.tiling = ((resource->mTotalReadFlags & RENDER_GRAPH_RESOURCE_READ_STATE_READBACK) != 0) ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;

						physRes->mDesc = resource->mDesc;

						mManager->GetRenderer()->createRenderTarget(&physRes->mRenderTarget, &desc);
					}
					else
					{
						physRes->mType = EPhysicalResourceType::PHYSICAL_RESOURCE_TYPE_TEXTURE;

						FImageCreateDesc desc;
						desc.format = resource->mDesc.mTextureDesc.format;
						desc.width = resource->mDesc.mTextureDesc.width;
						desc.height = resource->mDesc.mTextureDesc.height;
						desc.depth = resource->mDesc.mTextureDesc.depth;
						desc.arrayLayers = 1;
						desc.baseArrayLayer = 0;
						desc.baseMipLevel = 0;
						desc.mipLevels = 1;
						desc.pData = nullptr;
						desc.type = desc.depth != 1 ? EImageType::IMAGE_TYPE_3D : desc.height != 1 ? EImageType::IMAGE_TYPE_2D : EImageType::IMAGE_TYPE_1D;
						desc.usage = virtualResourceFlagsToTextureUsage(resource->mTotalReadFlags, resource->mTotalWriteFlags);
						desc.nativeSwapchainImage = nullptr;
						desc.sampleCount = resource->mDesc.mTextureDesc.sampleCount;
						desc.sampleQuality = resource->mDesc.mTextureDesc.sampleQuality;
						desc.memoryType = gResourceAccessToMemoryTypeTranslator(resource->mMaxResourceAccess);
						desc.tiling = ((resource->mTotalReadFlags & RENDER_GRAPH_RESOURCE_READ_STATE_READBACK) != 0) ? EImageTiling::IMAGE_TILING_LINEAR : EImageTiling::IMAGE_TILING_OPTIMAL;

						physRes->mDesc = resource->mDesc;

						mManager->GetRenderer()->createImage(&physRes->mImage, &desc);
					}

					// Register the physical resource.
					mTakenResources.push_back(physRes);
					mManager->mResources.insert(std::make_pair(std::unique_ptr<RPhysicalResource>(physRes), mManager->mFrameCounter));
					graph->mPhysicalResources[resource->mName] = physRes;
				}
			}
			break;
			default:
			{
				RLOG_WARNING("Unknown virtual resource type while trying to create physical resources.");
			}
			break;
			}
		}
	}

	bool RRenderGraphBuilder::isVirtualResourceCompatible(FVirtualResource* virtualResource, RPhysicalResource* physicalResource)
	{
		// These are not the same type therefore incompatible.
		if (virtualResource->mDesc.mType != physicalResource->mDesc.mType)
		{
			return false;
		}

		size_t hash1 = 0;
		size_t hash2 = 0;

		FVirtualTextureDesc desc = virtualResource->mDesc.GetTextureDesc();
		FVirtualTextureDesc desc2 = physicalResource->mDesc.GetTextureDesc();

		hash1 = hash_bytes(&desc, sizeof(FVirtualTextureDesc), hash1);
		hash2 = hash_bytes(&desc2, sizeof(FVirtualTextureDesc), hash2);

		return hash1 == hash2;

	}

	void RRenderGraphBuilder::addResourceBarrier(FVirtualResourceRef* to, RCompiledPass* pass, EBarrierLocation location)
	{
		RASSERT(to != nullptr);

		auto it = pass->mPhysicalResourceMappings.find(to->mParent == nullptr ? to->mName : to->mParent->mName);
		if (it != pass->mPhysicalResourceMappings.end())
		{
			RPhysicalResource* resource = it->second;

			switch (to->mResource->mDesc.mType)
			{
			case VIRTUAL_RESOURCE_TYPE_BUFFER:
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_BUFFER:
			{
				// Buffer barrier.
				FBufferBarrier barrier;
				barrier.buffer = resource->mBuffer;

				if (to->mReadFlags != 0)
				{
					barrier.requiredState = gReadFlagTranslator(to->mReadFlags);
				}
				else if (to->mWriteFlags != 0)
				{
					barrier.requiredState = gWriteFlagTranslator(to->mWriteFlags);
				}

				if (location == BARRIER_LOCATION_BEFORE)
				{
					pass->mBeforeBufferBarriers.push_back(barrier);
				}
				else
				{
					pass->mAfterBufferBarriers.push_back(barrier);
				}
			}
			break;
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET:
			{
				// Texture barrier.
				FImageBarrier barrier;
				barrier.image = resource->mRenderTarget->GetImage();

				if (to->mReadFlags != 0)
				{
					barrier.requiredState = gReadFlagTranslator(to->mReadFlags);
				}
				else if (to->mWriteFlags != 0)
				{
					barrier.requiredState = gWriteFlagTranslator(to->mWriteFlags);
				}


				if (location == BARRIER_LOCATION_BEFORE)
				{
					pass->mBeforeTextureBarriers.push_back(barrier);
				}
				else
				{
					pass->mAfterTextureBarriers.push_back(barrier);
				}

			}
			break;
			case VIRTUAL_RESOURCE_TYPE_TEXTURE:
			case VIRTUAL_RESOURCE_TYPE_IMPORTED_TEXTURE:
			{
				// Texture barrier.
				FImageBarrier barrier;
				if (resource->mType == EPhysicalResourceType::PHYSICAL_RESOURCE_TYPE_RENDER_TARGET)
				{
					barrier.image = resource->mRenderTarget->GetImage();
				}
				else
				{
					barrier.image = resource->mImage;
				}

				if (to->mReadFlags != 0)
				{
					barrier.requiredState = gReadFlagTranslator(to->mReadFlags);
				}
				else if (to->mWriteFlags != 0)
				{
					barrier.requiredState = gWriteFlagTranslator(to->mWriteFlags);
				}

				if (location == BARRIER_LOCATION_BEFORE)
				{
					pass->mBeforeTextureBarriers.push_back(barrier);
				}
				else
				{
					pass->mAfterTextureBarriers.push_back(barrier);
				}
			}
			break;
			default:
			{
				RLOG_WARNING("Unknown virtual resource type while trying to create barrier in render graph.");
			}
			break;
			}
		}
		else
		{
			RLOG_WARNING("Tried to create a barrier for a virtual resource but it has no physical resource attached.");
		}
	}


	/*! Do topological sort based on the dependencies. */
	void RRenderGraphBuilder::topological_sort(std::map<uint32_t, std::vector<uint32_t>>& dependencyMap, std::vector<uint32_t> &sortedPasses)
	{
		// Using Depth-first sorting (https://en.wikipedia.org/wiki/Topological_sorting)
		std::vector<bool> visited(mRenderPasses.size());

		for (uint32_t i = 0; i < mRenderPasses.size(); ++i)
		{
			if (!visited[i])
			{
				depthFirstSearch(i, visited, dependencyMap, sortedPasses);
			}
		}
	}

	void RRenderGraphBuilder::depthFirstSearch(uint32_t i, std::vector<bool>& visited, std::map<uint32_t, std::vector<uint32_t>>& dependencyMap, std::vector<uint32_t> &sortedPasses)
	{
		visited[i] = true;
		for (uint32_t j = 0; j < dependencyMap[i].size(); ++j)
		{
			uint32_t dependency = dependencyMap[i][j];

			if (!visited[dependency])
			{
				depthFirstSearch(dependency, visited, dependencyMap, sortedPasses);
			}
		}

		sortedPasses.push_back(i);
	}

	/*! Validate virtual passes. */
	EGraphBuilderResult RRenderGraphBuilder::validate_passes()
	{
		for (uint32_t i = 0; i < mRenderPasses.size(); ++i)
		{
			for (uint32_t j = 0; j < mRenderPasses[i].get()->mVirtualResourceReferences.size(); ++j)
			{
				FVirtualResourceRef* resource = mRenderPasses[i].get()->mVirtualResourceReferences[j].get();

				if (resource->mReadFlags != 0 && resource->mWriteFlags != 0)
				{
					RLOG_WARNING("Trying to read and write a resource in a pass.");
				}

				if ((resource->mReadFlags != 0) && ((resource->mWriteFlags & RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE) != 0))
				{
					RLOG_WARNING("Trying to read from a resource bound as depth write in a pass.");
				}

				if ((resource->mReadFlags != 0) && ((resource->mWriteFlags & RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT) != 0))
				{
					RLOG_WARNING("Trying to read from a resource bound as color attachment in a pass.");
				}
			}

			if (!mRenderPasses[i].get()->mSetupFunc && !mRenderPasses[i].get()->mExecuteFunc)
			{
				RLOG_WARNING("Render pass: \"%s\" has neither an setup or execute function, therefore you should consider removing this pass.", mRenderPasses[i].get()->mName);
			}
		}

		return EGraphBuilderResult::GRAPH_BUILDER_RESULT_SUCCESS;
	}

	void RRenderGraphBuilder::calculate_dependencies(std::map<uint32_t, std::vector<uint32_t>>& dependencyMap)
	{
		// Calculate dependencies for each render pass.
		for (uint32_t i = 0; i < mRenderPasses.size(); ++i)
		{
			for (uint32_t j = 0; j < mRenderPasses[i].get()->mVirtualResourceReferences.size(); ++j)
			{
				FVirtualResourceRef* resource = mRenderPasses[i].get()->mVirtualResourceReferences[j].get();

				uint32_t passIndex = getParentDependency(resource);

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

	IBuffer* RCompiledPass::getBufferResource(std::string resource)
	{
		auto it = mPhysicalResourceMappings.find(resource);
		if (it != mPhysicalResourceMappings.end())
		{
			RPhysicalResource* resource = it->second;

			if (resource->mType == PHYSICAL_RESOURCE_TYPE_BUFFER)
			{
				return resource->mBuffer;
			}
			else
			{
				RLOG_WARNING("Tried to get a buffer resource from the render graph but this resource is not a buffer.");
			}
		}
		return nullptr;
	}

	IImage* RCompiledPass::getTextureResource(std::string resource)
	{
		auto it = mPhysicalResourceMappings.find(resource);
		if (it != mPhysicalResourceMappings.end())
		{
			RPhysicalResource* resource = it->second;

			if (resource->mType == PHYSICAL_RESOURCE_TYPE_TEXTURE)
			{
				return resource->mImage;
			}
			else
			{
				RLOG_WARNING("Tried to get a image resource from the render graph but this resource is not a image.");
			}
		}
		return nullptr;
	}

	IRenderTarget* RCompiledPass::getRenderTargetResource(std::string resource)
	{
		auto it = mPhysicalResourceMappings.find(resource);
		if (it != mPhysicalResourceMappings.end())
		{
			RPhysicalResource* resource = it->second;

			if (resource->mType == PHYSICAL_RESOURCE_TYPE_RENDER_TARGET)
			{
				return resource->mRenderTarget;
			}
			else
			{
				RLOG_WARNING("Tried to get a render target resource from the render graph but this resource is not a render target.");
			}
		}
		return nullptr;
	}

	void RRenderGraph::execute(IRenderInterface* renderer)
	{
		ICommandList* list = mManager->getNewCommandList(ECommandType::COMMAND_TYPE_DIRECT);
		mGraphicsCommandLists.push_back(list);

		// Begin recording the command list.
		renderer->cmdBegin(list);

		// The passes should be sorted already so we can execute them sequentially.
		for (uint32_t i = 0; i < mPasses.size(); ++i)
		{
			RCompiledPass* pass = mPasses[i].get();

			// Add before resource barriers.
			if (!pass->mBeforeBufferBarriers.empty() || !pass->mBeforeTextureBarriers.empty())
			{
				renderer->cmdResourceBarriers(list, pass->mBeforeBufferBarriers.size(), pass->mBeforeBufferBarriers.data(), pass->mBeforeTextureBarriers.size(), pass->mBeforeTextureBarriers.data(), true);
			}

			if (pass->mExecuteFunc)
			{
				// Execute pass.
				pass->mExecuteFunc(renderer, *this, *pass, list);
			}

			// Add after resource barriers.
			if (!pass->mAfterBufferBarriers.empty() || !pass->mAfterTextureBarriers.empty())
			{
				renderer->cmdResourceBarriers(list, pass->mAfterBufferBarriers.size(), pass->mAfterBufferBarriers.data(), pass->mAfterTextureBarriers.size(), pass->mAfterTextureBarriers.data(), true);
			}
		}

	}

	void RRenderGraphManager::initialize(IRenderInterface* renderer, IQueue* graphicsQueue, IQueue* computeQueue)
	{
		mCurrentIndex = 0;
		mGraphicsQueue = graphicsQueue;
		mComputeQueue = computeQueue;
		mRenderer = renderer;
		mFrameCounter = 0;

		FCommandPoolCreateDesc poolDesc;
		poolDesc.type = ECommandType::COMMAND_TYPE_DIRECT;

		renderer->createCommandPool(&mGraphicsCommandPool, mGraphicsQueue, &poolDesc);

		poolDesc.type = ECommandType::COMMAND_TYPE_COMPUTE;
		renderer->createCommandPool(&mComputeCommandPool, mComputeQueue, &poolDesc);

		// Create synchronization primitives.
		for (uint32_t i = 0; i < 3; ++i)
		{
			renderer->createFence(&mRenderCompleteFences[i]);
			renderer->createSemaphore(&mRenderCompleteSemaphores[i]);
		}

		for (uint32_t i = 0; i < MAX_FRAMES_AT_ONCE; ++i)
		{
			mRenderGraphs[i] = new RRenderGraph(this);
			mRenderGraphs[i]->mRenderCompleteSemaphore = mRenderCompleteSemaphores[i];
			mRenderGraphs[i]->mRenderCompleteFence = mRenderCompleteFences[i];
		}
	}

	void RRenderGraphManager::destroy()
	{
		mRenderer->waitForDevice();

		for (auto it = mResources.begin(); it != mResources.end(); ++it)
		{
			RPhysicalResource* resource = it->first.get();

			switch (resource->mDesc.mType)
			{
			case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_BUFFER:
			{
				if (resource->mBuffer != nullptr)
				{
					mRenderer->destroyBuffer(resource->mBuffer);
				}
			}
			break;
			case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_TEXTURE:
			{
				if (resource->mType == EPhysicalResourceType::PHYSICAL_RESOURCE_TYPE_RENDER_TARGET)
				{
					if (resource->mRenderTarget != nullptr)
					{
						mRenderer->destroyRenderTarget(resource->mRenderTarget);
					}
				}
				else
				{
					if (resource->mImage != nullptr)
					{
						mRenderer->destroyImage(resource->mImage);
					}
				}
			}
			break;
			}
		}

		for (uint32_t i = 0; i < mGraphicsCommandLists.size(); ++i)
		{
			mRenderer->destroyCommandList(mGraphicsCommandPool, mGraphicsCommandLists[i]);
		}

		for (uint32_t i = 0; i < mComputeCommandLists.size(); ++i)
		{
			mRenderer->destroyCommandList(mComputeCommandPool, mComputeCommandLists[i]);
		}

		mRenderer->destroyCommandPool(mGraphicsCommandPool);
		mRenderer->destroyCommandPool(mComputeCommandPool);

		// Destroy synchronization primitives.
		for (uint32_t i = 0; i < 3; ++i)
		{
			mRenderer->destroyFence(mRenderCompleteFences[i]);
			mRenderer->destroySemaphore(mRenderCompleteSemaphores[i]);
		}

		for (uint32_t i = 0; i < MAX_FRAMES_AT_ONCE; ++i)
		{
			delete mRenderGraphs[i];
		}
	}

	ICommandList* RRenderGraphManager::getNewCommandList(ECommandType type)
	{
		switch (type)
		{
		case ECommandType::COMMAND_TYPE_DIRECT:
		{
			if (!mFreeGraphicsCommandLists.empty())
			{
				ICommandList* cmd = mFreeGraphicsCommandLists.front();
				mFreeGraphicsCommandLists.pop();

				return cmd;
			}
			else
			{
				ICommandList* cmd;
				mRenderer->createCommandList(&cmd, mGraphicsCommandPool);

				mGraphicsCommandLists.push_back(cmd);

				return cmd;
			}
		}
		break;
		case ECommandType::COMMAND_TYPE_COMPUTE:
		{
			if (!mFreeComputeCommandLists.empty())
			{
				ICommandList* cmd = mFreeComputeCommandLists.front();
				mFreeComputeCommandLists.pop();

				return cmd;
			}
			else
			{
				ICommandList* cmd;
				mRenderer->createCommandList(&cmd, mComputeCommandPool);

				mComputeCommandLists.push_back(cmd);

				return cmd;
			}
		}
		break;
		default:
		{
			RLOG_ERROR("Tried to get a new command list in the render graph system but it was not direct or compute.");
		}
		break;
		}


		return nullptr;
	}

	void RRenderGraph::reset()
	{
		for (ICommandList* list : mGraphicsCommandLists)
		{
			mManager->mFreeGraphicsCommandLists.push(list);
		}

		for (ICommandList* list : mComputeCommandLists)
		{
			mManager->mFreeComputeCommandLists.push(list);
		}

		mGraphicsCommandLists.clear();
		mComputeCommandLists.clear();
		mPhysicalResources.clear();
		mPasses.clear();
	}

	EGraphBuilderResult RRenderGraphManager::validate_and_build(RRenderGraphBuilder* pBuilder, RRenderGraph** ppGraph)
	{
		mCurrentIndex = (mCurrentIndex + 1) % MAX_FRAMES_AT_ONCE;

		RRenderGraph* retGraph = mRenderGraphs[mCurrentIndex];

		// Wait for the frame to finish.
		IFence* pNextFence = retGraph->mRenderCompleteFence;
		mRenderer->waitForFence(pNextFence);

		// We have to reset this graph from the previous state.
		retGraph->reset();

		// Validate that all the passes are compatible.
		EGraphBuilderResult validated = pBuilder->validate_passes();
		if (validated != GRAPH_BUILDER_RESULT_SUCCESS)
		{
			return validated;
		}

		// calculate dependencies for all passes.
		std::map<uint32_t, std::vector<uint32_t>> dependencyGraph;
		pBuilder->calculate_dependencies(dependencyGraph);

		// Vector of correctly sorted pass execution order.
		std::vector<uint32_t> sortedPasses;

		// Do topological sort.
		pBuilder->topological_sort(dependencyGraph, sortedPasses);

		// Create physical resources
		pBuilder->createPhysicalResources(retGraph);

		// Create physical passes.
		for (uint32_t i = 0; i < sortedPasses.size(); ++i)
		{
			RCompiledPass* pass = new RCompiledPass();
			FVirtualPass* virtualPass = pBuilder->mRenderPasses[sortedPasses[i]].get();

			pass->mExecuteFunc = virtualPass->mExecuteFunc;

			for (uint32_t j = 0; j < virtualPass->mVirtualResourceReferences.size(); ++j)
			{
				FVirtualResourceRef* ref = virtualPass->mVirtualResourceReferences[j].get();

				pass->mPhysicalResourceMappings[ref->mName] = retGraph->mPhysicalResources[ref->mResource->mName];

				if (ref->mParent == nullptr && ref->mWriteFlags != 0)
				{
					pBuilder->addResourceBarrier(ref, pass, BARRIER_LOCATION_BEFORE);
				}

				for (FVirtualResourceRef* refChild : ref->mChildren)
				{
					pBuilder->addResourceBarrier(refChild, pass, BARRIER_LOCATION_AFTER);
				}
			}

			// Add pass to the graph.
			retGraph->mPasses.push_back(std::unique_ptr<RCompiledPass>(pass));
		}

		*ppGraph = retGraph;
		delete pBuilder;

		return EGraphBuilderResult::GRAPH_BUILDER_RESULT_SUCCESS;
	}


	RRenderGraphBuilder* RRenderGraphManager::get_new_graph_builder()
	{
		mFrameCounter++;

		// Destroy resource that haven't been used for a full cycle.
		for (auto it = mResources.begin(); it != mResources.end();)
		{
			RPhysicalResource* resource = it->first.get();
			if (resource->mDesc.mType == EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_IMPORTED_BUFFER ||
				resource->mDesc.mType == EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_IMPORTED_TEXTURE ||
				resource->mDesc.mType == EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET)
			{
				mResources.erase(it++);
			}
			else if (mFrameCounter - it->second > MAX_FRAMES_AT_ONCE)
			{
				//mRenderer->waitForDevice();

				switch (resource->mDesc.mType)
				{
				case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_BUFFER:
				{
					mRenderer->destroyBuffer(resource->mBuffer);
				}
				break;
				case EVirtualResourceType::VIRTUAL_RESOURCE_TYPE_TEXTURE:
				{
					if (resource->mType == EPhysicalResourceType::PHYSICAL_RESOURCE_TYPE_RENDER_TARGET)
					{
						mRenderer->destroyRenderTarget(resource->mRenderTarget);
					}
					else
					{
						mRenderer->destroyImage(resource->mImage);
					}
				}
				break;
				}

				mResources.erase(it++);
			}
			else {
				++it;
			}
		}

		RRenderGraphBuilder* builder = new RRenderGraphBuilder(this);
		return builder;
	}

}
