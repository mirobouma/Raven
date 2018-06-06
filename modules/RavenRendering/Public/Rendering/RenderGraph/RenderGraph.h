#pragma once
#include <map>
#include <vector>
#include <unordered_map>
#include "Rendering/Interfaces/IRenderer.h"
#include <queue>
#include <functional>
#include <memory>

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

	/*! Enumeration describing if a barrier should be placed at the beginning of a pass or at the end. */
	typedef enum EBarrierLocation
	{
		BARRIER_LOCATION_BEFORE,
		BARRIER_LOCATION_AFTER
	} EBarrierLocation;

	/*! Read access flags when reading from a resource in a pass. */
	typedef enum FRenderGraphResourceReadState
	{
		RENDER_GRAPH_RESOURCE_READ_STATE_SAMPLED = 1 << 0,
		RENDER_GRAPH_RESOURCE_READ_STATE_DEPTH_READ = 1 << 1,
		RENDER_GRAPH_RESOURCE_READ_STATE_PRESENT = 1 << 2,
		MAX_RENDER_GRAPH_RESOURCE_READ_STATE = 3
	} FRenderGraphResourceReadState;
	DECL_ENUM_AS_FLAGS(FRenderGraphResourceReadState);

	/*! Write access flags when writing to a resource in a pass. */
	typedef enum FRenderGraphResourceWriteState
	{
		RENDER_GRAPH_RESOURCE_WRITE_STATE_UAV = 1 << 0,
		RENDER_GRAPH_RESOURCE_WRITE_STATE_COLOR_ATTACHMENT = 1 << 1,
		RENDER_GRAPH_RESOURCE_WRITE_STATE_DEPTH_WRITE = 1 << 2,
		RENDER_GRAPH_RESOURCE_WRITE_STATE_IMPORT = 1 << 3,
		RENDER_GRAPH_RESOURCE_WRITE_STATE_CREATE = 1 << 4,
		MAX_RENDER_GRAPH_RESOURCE_WRITE_STATE = 5
	} FRenderGraphResourceWriteState;
	DECL_ENUM_AS_FLAGS(FRenderGraphResourceWriteState);


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

		/*! Returns a virtual texture desc for texture targets. */
		FVirtualTextureDesc GetTextureDesc();
	};

	/*! Base structure for a virtual render graph resource. */
	struct FVirtualResource
	{
		FVirtualResourceDesc				mDesc;
		ERenderGraphResourceAccess			mMaxResourceAccess;
		FRenderGraphResourceReadState		mTotalReadFlags;
		FRenderGraphResourceWriteState		mTotalWriteFlags;
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
		FRenderGraphResourceReadState		mReadFlags; /*!< How do we read from this resource in this pass? */
		FRenderGraphResourceWriteState		mWriteFlags; /*!< How do we write to this resource in this pass? */
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
		friend class RRenderGraphBuilder;
		friend class RRenderGraphManager;
		friend class RRenderGraph;

	public:
		IBuffer* getBufferResource(std::string resource);
		ITexture* getTextureResource(std::string resource);
		IRenderTarget* getRenderTargetResource(std::string resource);

	protected:
		RRenderGraph*														mGraph;
		std::function<void(IRenderer*, RRenderGraph&, RCompiledPass&, ICommandList*)>	mExecuteFunc;
		std::map<std::string, RPhysicalResource*>							mPhysicalResourceMappings;
		std::vector<FTextureBarrier>										mBeforeTextureBarriers;
		std::vector<FBufferBarrier>											mBeforeBufferBarriers;
		std::vector<FTextureBarrier>										mAfterTextureBarriers;
		std::vector<FBufferBarrier>											mAfterBufferBarriers;
		std::vector<RCompiledPass*>											mNextPasses;
	};

	/*! Virtual Render pass graph */
	class FVirtualPass
	{
	protected:
		FVirtualPass(std::string name, std::function<void(RRenderPassBuilder&)> setupFunc) : mName(name), mSetupFunc(setupFunc) {}

	public:
		friend class RRenderPassBuilder;
		friend class RRenderGraphBuilder;
		friend class RRenderGraphManager;

		void setExecuteFunc(std::function<void(IRenderer*, RRenderGraph&, RCompiledPass&, ICommandList*)> execute)
		{
			mExecuteFunc = execute;
		}

	protected:
		bool mDoNotCull; // Flag describing if this render pass should never be culled.

		std::vector<FVirtualResourceRef*>									mInputs;
		std::vector<FVirtualResourceRef*>									mOutputs;

		std::function<void(RRenderPassBuilder&)>							mSetupFunc;
		std::function<void(IRenderer*, RRenderGraph&, RCompiledPass&, ICommandList*)>	mExecuteFunc;
		std::vector<std::unique_ptr<FVirtualResourceRef>>					mVirtualResourceReferences;

		std::string															mName;
		uint32_t															mIndex;
	};

	class RRenderGraphBuilder;
	class RRenderPassBuilder
	{
		friend class RRenderGraphManager;
	public:
		RRenderPassBuilder(RRenderGraphBuilder* graphBuilder, FVirtualPass* pass) : mGraphBuilder(graphBuilder), mPass(pass) {}

		FVirtualResourceRef* import_resource(std::string name, const FImportResourceDesc& desc); /*! Imports a resource in the render graph. */
		FVirtualResourceRef* create_resource(std::string name, const FVirtualResourceDesc& desc); /*! Create a resource in the render pass. */
		FVirtualResourceRef* read_resource(std::string name, FVirtualResourceRef* resource, ERenderGraphResourceAccess access, FRenderGraphResourceReadState readFlags); /*! Read from a resource. */
		FVirtualResourceRef* write_resource(std::string name, FVirtualResourceRef* resource, ERenderGraphResourceAccess access, FRenderGraphResourceWriteState writeFlags); /*! Write into a resource. */

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
		RRenderGraph(RRenderGraphManager* manager) : mManager(manager) {}
		~RRenderGraph() {}

		/*! Execute the compiled render graph. */
		void execute(IRenderer* renderer);

		/*! Reset to default state.*/
		void reset();

		std::map<std::string,	RPhysicalResource>		mPhysicalResources;
		std::vector<std::unique_ptr<RCompiledPass>>		mPasses;
		RRenderGraphManager*							mManager;
		ISemaphore*										mRenderCompleteSemaphore;
		IFence*											mRenderCompleteFence;

		std::vector<ICommandList*>						mGraphicsCommandLists;
		std::vector<ICommandList*>						mComputeCommandLists;
	};


	/*! Render Graph Builder, used to validate and build a render graph tree and dependency chain. */
	class RRenderGraphBuilder
	{
	public:
		friend class RRenderPassBuilder;
		friend class RRenderGraphManager;

		RRenderGraphBuilder(RRenderGraphManager* pManager) : mManager(pManager) {}
		~RRenderGraphBuilder();

		FVirtualPass* add_pass(std::string name, std::function<void(RRenderPassBuilder&)> setupFunc);

	protected:
		/*! Validation checks if the passes are correct. */
		EGraphBuilderResult validate_passes();

		/*! Calculate on which passes each pass depends. */
		void calculate_dependencies(std::map<uint32_t, std::vector<uint32_t>>& dependencyMap);

		/*! depth first search util*/
		void depthFirstSearch(uint32_t i, std::vector<bool>& visited, std::map<uint32_t, std::vector<uint32_t>>& dependencyMap, std::vector<uint32_t> &sortedPasses);

		/*! Create physical resources for this render graph. */
		void createPhysicalResources(RRenderGraph* graph);

		/*! Do topological sort based on the dependencies. */
		void topological_sort(std::map<uint32_t, std::vector<uint32_t>>& dependencyMap, std::vector<uint32_t> &sortedPasses);

		/*! Add resource barrier on a compiled render pass. */
		void addResourceBarrier(FVirtualResourceRef* to, RCompiledPass* pass, EBarrierLocation location);

		/*! Recursive function to retrieve a dependency for a resource. */
		uint32_t getParentDependency(FVirtualResourceRef* resource);

		RRenderGraphManager*										mManager; /*!< Manager this render graph belongs to. */
		std::map<std::string, std::unique_ptr<FVirtualResource>>	mVirtualResources; /*!< Vector of virtual resources in this render graph. */
		std::vector<std::unique_ptr<FVirtualPass>>					mRenderPasses; /*!< Map of virtual resources in this render graph. */

	};
	

	/*!< Manager object for render graphs. */
	class RRenderGraphManager
	{
	public:
		friend class RRenderGraph;

		/*! Initialize the graph manager. */
		void initialize(IRenderer* renderer, IQueue* graphicsQueue, IQueue* computeQueue);
	
		/*! Destroy the graph manager. */
		void destroy();
		
		/*! Get graphics and compute queues*/
		IQueue* GetGraphicsQueue() const { return mGraphicsQueue; }
		IQueue* GetComputeQueue() const { return mComputeQueue; }
		IRenderer* GetRenderer() const { return mRenderer; }
		RRenderGraphBuilder* get_new_graph_builder(); /*!< Retrieve a new render graph builder. Should only be called once per frame. */

		/*! Compiles all registered modules and resolves dependencies into a render graph.*/
		EGraphBuilderResult validate_and_build(RRenderGraphBuilder* pBuilder, RRenderGraph** ppGraph);

	protected:

		ICommandList* getNewCommandList(ECommandType type); /*!< Get a command list from the pool. */

	protected:
		IRenderer*		mRenderer; /*!< Renderer instance to which we have to submit commands. */

		ICommandPool*	mGraphicsCommandPool; /*!< Command pool for graphics commands for the render graphs. */
		ICommandPool*	mComputeCommandPool; /*!< Command pool for the compute commands for the render graphs. */

		IQueue*			mGraphicsQueue; /*!< Queue to submit graphics commands to. */
		IQueue*			mComputeQueue; /*!< Queue to submit compute commands to. */

		uint32_t mCurrentIndex; /*! Current frame index. */

		ISemaphore* mRenderCompleteSemaphores[MAX_FRAMES_AT_ONCE]; /*!< Render complete fences. */
		IFence* mRenderCompleteFences[MAX_FRAMES_AT_ONCE]; /*!< Render complete fences. */

		std::queue<ICommandList*> mFreeGraphicsCommandLists; /*!< Queue of free command lists (recycled). */
		std::vector<ICommandList*> mGraphicsCommandLists; /*!< Vector of all existing command lists. */

		std::queue<ICommandList*> mFreeComputeCommandLists; /*!< Queue of free command lists (recycled). */
		std::vector<ICommandList*> mComputeCommandLists; /*!< Vector of all existing command lists. */

		RRenderGraph* mRenderGraphs[MAX_FRAMES_AT_ONCE]; /*!< array of render graphs per frame. */
	};
}