#pragma once
#include <map>
#include <vector>
#include <unordered_map>
#include "Rendering/Interfaces/IRenderer.h"
#include <queue>
#include <functional>

namespace Raven
{
	// Forward declarations
	class RRenderGraph;
	class RRenderPassBuilder;

	/*! Result from building a render graph. */
	typedef enum EGraphBuilderResult
	{
		GRAPH_BUILDER_RESULT_FAILURE,
		GRAPH_BUILDER_RESULT_SUCCESS,
		GRAPH_BUILDER_RESULT_INCORRECT_ATTACHMENT_COUNT,
		GRAPH_BUILDER_RESULT_INCOMPATIBLE_ATTACHMENTS,
		MAX_GRAPH_BUILDER_RESULT
	}EGraphBuilderResult;
	
	/*! Type of virtual resource. */
	typedef enum EVirtualResourceType
	{
		VIRTUAL_RESOURCE_TYPE_BUFFER,
		VIRTUAL_RESOURCE_TYPE_TEXTURE,
		VIRTUAL_RESOURCE_TYPE_IMPORTED_BUFFER,
		VIRTUAL_RESOURCE_TYPE_IMPORTED_TEXTURE,
		VIRTUAL_RESOURCE_TYPE_IMPORTED_RENDER_TARGET,
		MAX_VIRTUAL_RESOURCE_TYPE
	} EVirtualResourceType;

	/*! Type of physical resource. */
	typedef enum EPhysicalResourceType
	{
		PHYSICAL_RESOURCE_TYPE_BUFFER,
		PHYSICAL_RESOURCE_TYPE_TEXTURE,
		PHYSICAL_RESOURCE_TYPE_RENDER_TARGET,
		MAX_PHYSICAL_TYPE
	} EPhysicalResourceType;

	/*! Access patterns for a specific resource of reference. */
	typedef enum ERenderGraphResourceAccess 
	{
		RENDER_GRAPH_RESOURCE_ACCESS_GPU,
		RENDER_GRAPH_RESOURCE_ACCESS_CPU,
		RENDER_GRAPH_RESOURCE_ACCESS_GPU_AND_CPU,
		MAX_RENDER_GRAPH_RESOURCE_ACCESS
	} ERenderGraphResourceAccess;

	/*! Read access flags when reading from a resource in a pass. */
	typedef enum FRenderGraphResourceReadFlags
	{
		RENDER_GRAPH_RESOURCE_READ_FLAG_SAMPLED = 1 << 0,
		RENDER_GRAPH_RESOURCE_READ_FLAG_DEPTH_READ = 1 << 1,
		MAX_RENDER_GRAPH_RESOURCE_READ_FLAG = 2
	} FRenderGraphResourceReadFlags;
	DECL_ENUM_AS_FLAGS(FRenderGraphResourceReadFlags);

	/*! Write access flags when writing to a resource in a pass. */
	typedef enum FRenderGraphResourceWriteFlags
	{
		RENDER_GRAPH_RESOURCE_WRITE_FLAG_UAV = 1 << 0,
		RENDER_GRAPH_RESOURCE_WRITE_FLAG_COLOR_ATTACHMENT = 1 << 1,
		RENDER_GRAPH_RESOURCE_WRITE_FLAG_DEPTH_WRITE = 1 << 2,
		MAX_RENDER_GRAPH_RESOURCE_USAGE = 3
	} FRenderGraphResourceWriteFlags;
	DECL_ENUM_AS_FLAGS(FRenderGraphResourceWriteFlags);


	/*! \struct FImportResourceDesc
	Structure for describing an external dependency on a resource outside the render graph. (Eg. Backbuffer). 
	External dependencies must make sure they have the correct usage flags. */
	struct FImportResourceDesc
	{
		EVirtualResourceType mType;

		/*! Union of resource pointers, uses different resource depending on mType. */
		union 
		{
			IBuffer* pBuffer;
			ITexture* pTexture;
			IRenderTarget* pRenderTarget;
		};
	};

	/*! Structure describing a virtual buffer used in the render graph. */
	struct FVirtualBufferDesc
	{
		size_t size; /*!< Size of buffer. */
	};

	/*! Structure describing a virtual texture for usage in the render graph. */
	struct FVirtualTextureDesc
	{
		size_t				width; /*!< Width of this texture. */
		size_t				height; /*!< Height of this texture. */
		size_t				depth; /*!< Depth of this texture. */
		uint32_t			sampleCount; /*!< Amount of samples this texture has. */
		uint32_t			sampleQuality; /*!< Quality of the samples for this texture. */
		ETextureFormat		format; /*!< Format of this texture. */
	};


	/*! Structure describing a virtual resource, and wether it's an imported target or not. */
	struct FVirtualResourceDesc
	{
		EVirtualResourceType mType;

		union
		{
			FVirtualBufferDesc  mBufferDesc;
			FVirtualTextureDesc mTextureDesc;
			IBuffer*			mImportedBuffer;
			IRenderTarget*		mImportedRenderTarget;
			ITexture*			mImportedTexture;
		};
	};

	/*! Base structure for a virtual render graph resource. */
	struct FVirtualResource
	{
		FVirtualResourceDesc				mDesc;
		ERenderGraphResourceAccess			mMaxResourceAccess;
		FRenderGraphResourceReadFlags		mTotalReadFlags;
		FRenderGraphResourceWriteFlags		mTotalWriteFlags;
		uint32_t							mCreatedInPass;
		std::string                         mName;
		std::vector<uint32_t>				mReferencedInPasses;
	};

	struct FVirtualResourceRef
	{
		std::string							mName; /*!< Name of this virtual resource reference. */
		FVirtualResourceRef*				mParent; /*!< Parent resource. */
		FVirtualResource*					mResource; /*!< Pointer to the virtual resource we are referencing in this pass. */
		ERenderGraphResourceAccess			mResourceAccess; /*!< Do we access the resource on the CPU, GPU, or both? */
		FRenderGraphResourceReadFlags		mReadFlags; /*!< How do we read from this resource in this pass? */
		FRenderGraphResourceWriteFlags		mWriteFlags; /*!< How do we write to this resource in this pass? */
		std::vector<FVirtualResourceRef*>	mChildren; /*!< Which following passes write to this resource? */
		uint32_t							mPassIndex; /*!< In which pass do we use this resource reference? */
	};

	struct RPhysicalResource
	{
		EPhysicalResourceType mType;

		union
		{
			IBuffer*			mBuffer;
			IRenderTarget*		mRenderTarget;
			ITexture*			mTexture;
		};
	};

	class RCompiledPass
	{
	public:
		IBuffer* getBufferResource(std::string resource);
		ITexture* getTextureResource(std::string resource);
		IRenderTarget* getRenderTargetResource(std::string resource);

	protected:
		RRenderGraph * mGraph;
		std::map<std::string, RPhysicalResource*>	mPhysicalResourceMappings;
	};

	/*! Virtual Render pass graph */
	class FVirtualPass
	{
	protected:
		FVirtualPass(std::string name, std::function<void(RRenderPassBuilder&)> setupFunc) : mName(name), mSetupFunc(setupFunc) {}

	public:
		friend class RRenderPassBuilder;
		friend class RRenderGraphBuilder;

		void setExecuteFunc(std::function<void(RRenderGraph&, RCompiledPass&)> execute)
		{
			mExecuteFunc = execute;
		}

	protected:
		bool mDoNotCull; // Flag describing if this render pass should never be culled.

		std::vector<FVirtualResourceRef*>					mInputs;
		std::vector<FVirtualResourceRef*>					mOutputs;

		std::function<void(RRenderPassBuilder&)>			mSetupFunc;
		std::function<void(RRenderGraph&, RCompiledPass&)>	mExecuteFunc;
		std::vector<FVirtualResourceRef>					mVirtualResourceReferences;

		std::string											mName;
		uint32_t											mIndex;
	};

	class RRenderGraphBuilder;
	class RRenderPassBuilder
	{
	public:
		RRenderPassBuilder(RRenderGraphBuilder* graphBuilder, FVirtualPass* pass) : mGraphBuilder(graphBuilder), mPass(pass) {}

		FVirtualResourceRef* import_resource(std::string name, const FImportResourceDesc& desc); /*! Imports a resource in the render graph. */
		FVirtualResourceRef* create_resource(std::string name, const FVirtualResourceDesc& desc); /*! Create a resource in the render pass. */
		FVirtualResourceRef* read_resource(std::string name, FVirtualResourceRef* resource, ERenderGraphResourceAccess access, FRenderGraphResourceReadFlags readFlags); /*! Read from a resource. */
		FVirtualResourceRef* write_resource(std::string name, FVirtualResourceRef* resource, ERenderGraphResourceAccess access, FRenderGraphResourceWriteFlags writeFlags); /*! Write into a resource. */

		void setDoNotCull(bool doNotCull)
		{
			mPass->mDoNotCull = doNotCull;
		}

	protected:
		RRenderGraphBuilder* mGraphBuilder;
		FVirtualPass*		 mPass;
	};

	class RRenderGraphManager;

	/*! \class FRenderGraph
	\brief Rendering graph abstraction to simplify rendering.
	*/
	class RRenderGraph
	{
	public:
		RRenderGraph() {}
		~RRenderGraph() {}

		/*! Execute the compiled render graph. */
		void execute();

		std::map<std::string,	RPhysicalResource> mPhysicalResources;
		std::map<size_t,		RCompiledPass>	   mPasses;
		RRenderGraphManager*					   mManager;
	};


	/*! Render Graph Builder, used to validate and build a render graph tree and dependency chain. */
	class RRenderGraphBuilder
	{
	public:
		friend class RRenderPassBuilder;

		FVirtualPass* add_pass(std::string name, std::function<void(RRenderPassBuilder&)> setupFunc);


		/*! Compiles all registered modules and resolves dependencies into a render graph.*/
		EGraphBuilderResult validate_and_build(RRenderGraph** graph);

	protected:
		/*! Validation checks if the passes are correct. */
		EGraphBuilderResult validate_passes();

		/*! Calculate on which passes each pass depends. */
		void calculate_dependencies(std::map<uint32_t, std::vector<uint32_t>>& dependencyMap);

		/*! Recursive function to retrieve a dependency for a resource. */
		uint32_t getParentDependency(FVirtualResourceRef* resource);

		RRenderGraphManager*										mManager; /*!< Manager this render graph belongs to. */
		std::map<std::string, FVirtualResource>						mVirtualResources; /*!< Vector of virtual resources in this render graph. */
		std::vector<FVirtualPass>									mRenderPasses; /*!< Map of virtual resources in this render graph. */

	};
	//class RRenderGraphManager;

	

	/*!< Manager object for render graphs. */
	class RRenderGraphManager
	{
	public:
		/*! Initialize the graph manager. */
		static void initialize();
	
		/*! Destroy the graph manager. */
		static void destroy();
	
		RRenderGraphBuilder*	get_new_graph_builder(); /*!< Retrieve a new render graph builder. Should only be called once per frame. */
		void					cleanup_graph(RRenderGraphBuilder* graph); /*! Clean up any allocated resource for the specified render graph. */
	
	protected:
		std::queue<RRenderGraphBuilder> mBuildersInFlight; /*!< Queue of render graph builders in flight. */
	};

	extern RRenderGraphManager* GRenderGraphManager;
}