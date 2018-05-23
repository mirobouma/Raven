#pragma once
#include <map>
#include <vector>
#include <unordered_map>
#include "Rendering/Interfaces/IRenderer.h"
#include <queue>
#include <functional>

namespace Raven
{
	struct FVirtualResource;
	typedef struct FRenderGraphResource
	{
		FVirtualResource* mVirtualResource;
		uint32_t version;
		std::string name;	
	} FRenderGraphResource;

	typedef enum EGraphBuilderResult
	{
		GRAPH_BUILDER_RESULT_FAILURE,
		GRAPH_BUILDER_RESULT_SUCCESS,
		GRAPH_BUILDER_RESULT_INCORRECT_ATTACHMENT_COUNT,
		GRAPH_BUILDER_RESULT_INCOMPATIBLE_ATTACHMENTS,
		MAX_GRAPH_BUILDER_RESULT
	}EGraphBuilderResult;
	
	typedef enum ERenderGraphResourceType
	{
		RENDER_GRAPH_RESOURCE_TYPE_BUFFER,
		RENDER_GRAPH_RESOURCE_TYPE_TEXTURE,
		RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_BUFFER,
		RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_TEXTURE,
		RENDER_GRAPH_RESOURCE_TYPE_IMPORTED_RENDER_TARGET,
		MAX_RENDER_GRAPH_RESOURCE_TYPE
	} ERenderGraphResourceType;

	typedef enum ERenderGraphResourceAccess 
	{
		RENDER_GRAPH_RESOURCE_ACCESS_GPU,
		RENDER_GRAPH_RESOURCE_ACCESS_CPU,
		RENDER_GRAPH_RESOURCE_ACCESS_GPU_AND_CPU,
		MAX_RENDER_GRAPH_RESOURCE_ACCESS
	} ERenderGraphResourceAccess;

	typedef enum FRenderGraphResourceReadFlags
	{
		RENDER_GRAPH_RESOURCE_READ_FLAG_SAMPLED = 1 << 0,
		RENDER_GRAPH_RESOURCE_READ_FLAG_DEPTH_READ = 1 << 1,
		RENDER_GRAPH_RESOURCE_READ_FLAG_DEPTH_WRITE = 1 << 2,
		MAX_RENDER_GRAPH_RESOURCE_READ_FLAG = 3
	} FRenderGraphResourceReadFlags;
	DECL_ENUM_AS_FLAGS(FRenderGraphResourceReadFlags);

	typedef enum FRenderGraphResourceWriteFlags
	{
		RENDER_GRAPH_RESOURCE_WRITE_FLAG_UAV = 1 << 0,
		RENDER_GRAPH_RESOURCE_WRITE_FLAG_COLOR_ATTACHMENT = 1 << 1,
		RENDER_GRAPH_RESOURCE_WRITE_FLAG_DEPTH_WRITE = 1 << 2,
		MAX_RENDER_GRAPH_RESOURCE_USAGE = 3
	} FRenderGraphResourceWriteFlags;
	DECL_ENUM_AS_FLAGS(FRenderGraphResourceWriteFlags);

	class RRenderPassBuilder;

	/*! \struct FImportResourceDesc
	Structure for describing an external dependency on a resource outside the render graph. (Eg. Backbuffer). 
	External dependencies must make sure they have the correct usage flags. */
	struct FImportResourceDesc
	{
		ERenderGraphResourceType mType;

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


	/*! Base structure for a virtual render graph resource. */
	struct FVirtualResourceDesc
	{
		ERenderGraphResourceType mType;

		union
		{
			FVirtualBufferDesc  mBufferDesc;
			FVirtualTextureDesc mTextureDesc;
			IBuffer*			mImportedBuffer;
			IRenderTarget*		mImportedRenderTarget;
			ITexture*			mImportedTexture;
		};
	};

	struct FVirtualResource
	{
		FVirtualResourceDesc mDesc;
		ERenderGraphResourceAccess mMaxResourceAccess;
		FRenderGraphResourceReadFlags mTotalReadFlags;
		FRenderGraphResourceWriteFlags mTotalWriteFlags;
		uint32_t mMaxVersion;
	};

	struct FVirtualResourceRef
	{
		FVirtualResource*				mResource;
		ERenderGraphResourceAccess		mResourceAccess;
		FRenderGraphResourceReadFlags	mReadFlags;
		FRenderGraphResourceWriteFlags	mWriteFlags;
	};
	/*! Render pass graph */
	class FBaseRenderGraphPass
	{
		friend class RRenderPassBuilder;
		friend class RRenderGraphBuilder;

	protected:
		bool								mDoNotCull;
		std::vector<FVirtualResourceRef>	mVirtualResourceReferences;
	};

	class RRenderGraph;

	/*! Render pass graph */
	template <typename T>
	class FRenderGraphPass : public FBaseRenderGraphPass
	{
	public:
		friend class RRenderGraphBuilder;

		virtual void setup_pass(RRenderPassBuilder& builder, T& data) = 0;
		virtual void execute_pass(RRenderGraph& graph, const T& data) = 0;
		virtual const std::string& get_name() const = 0;
	};

	class RRenderPassBuilder;

	template <typename T>
	class TLambdaRenderGraphPass : public FRenderGraphPass<T>
	{
	public:
		TLambdaRenderGraphPass(std::string name, std::function<void(RRenderPassBuilder&, T&)> setup, std::function<void(RRenderGraph&, const T&)> execute) : FRenderGraphPass<T>()
		{
			mName = name;
			mSetupFunc = setup;
			mExecuteFunc = execute;
		}

		virtual void setup_pass(RRenderPassBuilder& builder, T& data) override
		{
			mSetupFunc(builder, data);
		}

		virtual void execute_pass(RRenderGraph& graph, const T& data) override
		{
			mExecuteFunc(graph, data);
		}

		virtual const std::string& get_name() const
		{
			return mName;
		}

	protected:
		std::function<void(RRenderPassBuilder&, T&)>					mSetupFunc;
		std::function<void(RRenderGraph&, const T&)>					mExecuteFunc;
		std::string														mName;
	};

	/*! \class FRenderGraph
	\brief Rendering graph abstraction to simplify rendering.
	*/
	class RRenderGraph
	{
	public:
		RRenderGraph();
		~RRenderGraph();

		IBuffer* getBufferResource(const FRenderGraphResource* resource);
		ITexture* getTextureResource(const FRenderGraphResource* resource);
		IRenderTarget* getRenderTargetResource(const FRenderGraphResource* resource);

		/*! Execute the compiled render graph. */
		void execute_graph();

		std::map<size_t, FBaseRenderGraphPass*> mPasses;
	};

	class RRenderGraphBuilder;
	class RRenderPassBuilder
	{
	public:
		RRenderPassBuilder(RRenderGraphBuilder* graphBuilder, FBaseRenderGraphPass* pass) : mGraphBuilder(graphBuilder), mPass(pass) {}

		FRenderGraphResource import_resource(std::string name, const FImportResourceDesc& desc); /*! Imports a resource in the render graph. */
		FRenderGraphResource create_resource(std::string name, const FVirtualResourceDesc& desc); /*! Create a resource in the render pass. */
		FRenderGraphResource read_resource(FRenderGraphResource resource, ERenderGraphResourceAccess access, FRenderGraphResourceReadFlags readFlags); /*! Read from a resource. */
		FRenderGraphResource write_resource(FRenderGraphResource resource, ERenderGraphResourceAccess access, FRenderGraphResourceWriteFlags writeFlags); /*! Write into a resource. */

		bool mDoNotCull;

	protected:
		RRenderGraphBuilder*									mGraphBuilder;
		FBaseRenderGraphPass*									mPass;
	};

	class RRenderGraphManager;

	/*! Render Graph Builder, used to validate and build a render graph tree and dependency chain. */
	class RRenderGraphBuilder
	{
	public:
		friend class RRenderPassBuilder;

		/*! Register a module to the builder. */
		template <typename T>
		T add_lambda_render_pass(std::string name, std::function<void(RRenderPassBuilder&, T&)> setup, std::function<void(RRenderGraph&, const T&)> execute)
		{
			T output;

			// Create pass and set up.
			TLambdaRenderGraphPass<T> pass = TLambdaRenderGraphPass<T>(name, setup, execute);
			RRenderPassBuilder builder(this, &pass);

			pass.setup_pass(builder, output);

			if (builder.mDoNotCull)
			{
				pass.mDoNotCull = true;
			}

			mRenderPassMap[name] = pass;

			return output;
		}

		/*! Compiles all registered modules and resolves dependencies into a render graph.*/
		EGraphBuilderResult validate_and_build(RRenderGraph* graph);
		
	protected:

		RRenderGraphManager*									mManager; /*!< Manager this render graph belongs to. */
		std::unordered_map<std::string, FVirtualResource>		mVirtualResourceMap; /*!< Map of virtual resources in this render graph. */
		std::map<std::string, FBaseRenderGraphPass>				mRenderPassMap; /*!< Map of virtual resources in this render graph. */

	};

	/*! Manager object for render graphs. */
	class RRenderGraphManager
	{
		/*! Initialize the graph manager. */
		void initialize();

		/*! Destroy the graph manager. */
		void destroy();

		RRenderGraphBuilder*	get_new_graph_builder(); /*!< Retrieve a new render graph builder. Should only be called once per frame. */
		void					cleanup_graph_resource(RRenderGraphBuilder* graph); /*! Clean up any allocated resource for the specified render graph. */

	protected:
		std::queue<RRenderGraphBuilder> mBuildersInFlight; /*!< Queue of render graph builders in flight. */
	};
}