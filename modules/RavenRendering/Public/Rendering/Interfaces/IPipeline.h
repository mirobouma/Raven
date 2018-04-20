#pragma once
#include <cinttypes>

namespace Raven
{
	// Forward declarations.
	struct IRootSignature;
	struct IShader;
	struct IRenderTarget;

	/*! \struct FRasterizerState
	\brief Structure describing rasterizer state. */
	typedef struct FRasterizerState
	{

	} FRasterizerState;

	/*! \struct FDepthStencilState
	\brief Structure describing depth-stencil state. */
	typedef struct FDepthStencilState
	{

	} FDepthStencilState;

    /*! \struct FBlendState
    \brief Structure describing blend state. */
    typedef struct FBlendState
    {

    } FBlendState;

	/*! \enum EPrimitiveTopology
	\brief Enumeration of primitive topology types for drawing. */
	typedef enum EPrimitiveTopology
	{
		PRIMITIVE_TOPOLOGY_POINT,
		PRIMITIVE_TOPOLOGY_LINE,
		PRIMITIVE_TOPOLOGY_TRIANGLE,
		PRIMITIVE_TOPOLOGY_PATCH,
		MAX_PRIMITIVE_TOPOLOGY
	} EPrimitiveTopology;

	/*! \struct FGraphicsPipelineCreateDesc
	\brief Structure describing the parameters for graphics pipeline creation. */
	typedef struct FGraphicsPipelineCreateDesc
	{
		// === DirectX ===
		//ID3D12RootSignature *pRootSignature;
		//D3D12_SHADER_BYTECODE VS;
		//D3D12_SHADER_BYTECODE PS;
		//D3D12_SHADER_BYTECODE DS;
		//D3D12_SHADER_BYTECODE HS;
		//D3D12_SHADER_BYTECODE GS;
		//D3D12_STREAM_OUTPUT_DESC StreamOutput;
		//D3D12_BLEND_DESC BlendState;
		//UINT SampleMask;
		//D3D12_RASTERIZER_DESC RasterizerState;
		//D3D12_DEPTH_STENCIL_DESC DepthStencilState;
		//D3D12_INPUT_LAYOUT_DESC InputLayout;
		//D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
		//D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
		//UINT NumRenderTargets;
		//DXGI_FORMAT RTVFormats[8];
		//DXGI_FORMAT DSVFormat;
		//DXGI_SAMPLE_DESC SampleDesc;
		//UINT NodeMask;
		//D3D12_CACHED_PIPELINE_STATE CachedPSO;
		//D3D12_PIPELINE_STATE_FLAGS Flags;

		// === Vulkan ===
		//VkPipelineCreateFlags                            flags;
		//uint32_t                                         stageCount;
		//const VkPipelineShaderStageCreateInfo*           pStages;
		//const VkPipelineVertexInputStateCreateInfo*      pVertexInputState;
		//const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState;
		//const VkPipelineTessellationStateCreateInfo*     pTessellationState;
		//const VkPipelineViewportStateCreateInfo*         pViewportState;
		//const VkPipelineRasterizationStateCreateInfo*    pRasterizationState;
		//const VkPipelineMultisampleStateCreateInfo*      pMultisampleState;
		//const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState;
		//const VkPipelineColorBlendStateCreateInfo*       pColorBlendState;
		//const VkPipelineDynamicStateCreateInfo*          pDynamicState;
		//VkPipelineLayout                                 layout;
		//VkRenderPass                                     renderPass;
		//uint32_t                                         subpass;
		//VkPipeline                                       basePipelineHandle;
		//int32_t                                          basePipelineIndex;

		// === Shared ===
		// Flags, not neccessary for user.
		// Root signature
		// Input Layout
		// Rasterizer State
		// Multisample state
		// Depth stencil state
		// Blend state
		// Primitive topology
		// Render target formats
		// Depth stencil formats

		IRootSignature*			pRootSignature;
		IShader*				pShader;
		uint32_t 				numRenderTargets;
		IRenderTarget*			pRenderTargets;
		IRenderTarget*			pDepthStencilTarget;
		FRasterizerState*		pRasterizerState;
		FDepthStencilState*		pDepthStencilState;
		FBlendState*			pBlendState;
		uint32_t				sampleCount;
		uint32_t				sampleQuality;
		EPrimitiveTopology		primitiveTopology;
	} FGraphicsPipelineCreateDesc;

	/*! \struct FComputePipelineCreateDesc
	\brief Structure describing the parameters for graphics pipeline creation. */
	typedef struct FComputePipelineCreateDesc
	{
		IRootSignature*			pRootSignature;
		IShader*				pShader;
	} FComputePipelineCreateDesc;

	/*! \struct IPipeline
	\brief Interface for a pipeline state object. */
	struct IPipeline {};

	/*! \struct IGraphicsPipeline
	\brief Interface for a graphics pipeline state object. */
	struct IGraphicsPipeline : public IPipeline {};

	/*! \struct IComputePipeline
	\brief Interface for a compute pipeline state object. */
	struct IComputePipeline : public IPipeline {};
}