#pragma once
#include <cinttypes>
#include "core/api/rendering/defines.h"

struct IRootSignature;
struct IShader;
struct IRenderTarget;

typedef enum PolygonMode
{
    POLYGON_MODE_FILL,
    POLYGON_MODE_LINE,
    POLYGON_MODE_POINT,
    MAX_POLYGON_MODE
} PolygonMode;

typedef enum CullMode
{
    CULL_MODE_NONE,
    CULL_MODE_FRONT,
    CULL_MODE_BACK,
    CULL_MODE_FRONT_AND_BACK,
    MAX_CULL_MODE
} CullMode;

typedef enum FrontFaceWinding
{
    FRONT_FACE_WINDING_COUNTER_CLOCKWISE,
    FRONT_FACE_WINDING_CLOCKWISE,
    MAX_FRONT_FACE_WINDING
} FrontFaceWinding;

typedef enum CompareOp
{
    COMPARE_OP_NEVER,
    COMPARE_OP_LESS,
    COMPARE_OP_EQUAL,
    COMPARE_OP_LESS_OR_EQUAL,
    COMPARE_OP_GREATER,
    COMPARE_OP_NOT_EQUAL,
    COMPARE_OP_GREATER_OR_EQUAL,
    COMPARE_OP_ALWAYS,
    MAX_COMPARE_OP
} CompareOp;

typedef enum BlendFactor
{
    BLEND_FACTOR_ZERO,
    BLEND_FACTOR_ONE,
    BLEND_FACTOR_SRC_COLOR,
    BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    BLEND_FACTOR_DST_COLOR,
    BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    BLEND_FACTOR_SRC_ALPHA,
    BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    BLEND_FACTOR_DST_ALPHA,
    BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    BLEND_FACTOR_CONSTANT_COLOR,
    BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
    BLEND_FACTOR_CONSTANT_ALPHA,
    BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
    BLEND_FACTOR_SRC_ALPHA_SATURATE,
    BLEND_FACTOR_SRC1_COLOR,
    BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
    BLEND_FACTOR_SRC1_ALPHA,
    BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA,
    MAX_BLEND_FACTOR
} BlendFactor;

typedef enum BlendOp
{
    BLEND_OP_ADD,
    BLEND_OP_SUBTRACT,
    BLEND_OP_REVERSE_SUBTRACT,
    BLEND_OP_MIN,
    BLEND_OP_MAX,
    MAX_BLEND_OP
} BlendOp;

typedef enum LogicOp
{
    LOGIC_OP_CLEAR,
    LOGIC_OP_AND,
    LOGIC_OP_AND_REVERSE,
    LOGIC_OP_COPY,
    LOGIC_OP_AND_INVERTED,
    LOGIC_OP_NO_OP,
    LOGIC_OP_XOR,
    LOGIC_OP_OR,
    LOGIC_OP_NOR,
    LOGIC_OP_EQUIVALENT,
    LOGIC_OP_INVERT,
    LOGIC_OP_OR_REVERSE,
    LOGIC_OP_COPY_INVERTED,
    LOGIC_OP_OR_INVERTED,
    LOGIC_OP_NAND,
    LOGIC_OP_SET,
    MAX_LOGIC_OP
} LogicOp;

typedef enum StencilOp
{
    STENCIL_OP_KEEP,
    STENCIL_OP_ZERO,
    STENCIL_OP_REPLACE,
    STENCIL_OP_INCREMENT_AND_CLAMP,
    STENCIL_OP_DECREMENT_AND_CLAMP,
    STENCIL_OP_INVERT,
    STENCIL_OP_INCREMENT_AND_WRAP,
    STENCIL_OP_DECREMENT_AND_WRAP,
    MAX_STENCIL_OP
} StencilOp;

typedef struct StencilOpState
{
    StencilOp		failOp;
    StencilOp		passOp;
    StencilOp		depthFailOp;
    CompareOp		compareOp;
    uint32_t		compareMask;
    uint32_t		writeMask;
} StencilOpState;

typedef struct RasterizerState
{
    bool				depthClampEnable;
    bool				rasterizerDiscardEnable;
    PolygonMode		    polygonMode;
    float				lineWidth;
    CullMode			cullMode;
    FrontFaceWinding	frontFace;
    bool				depthBiasEnable;
    float				depthBiasConstantFactor;
    float				depthBiasClamp;
    float				depthBiasSlopeFactor;
} RasterizerState;

typedef struct DepthStencilState
{
    bool				depthTestEnable;
    bool				depthWriteEnable;
    CompareOp			depthCompareOp;
    bool				depthBoundsTestEnable;
    float				minDepthBounds;
    float				maxDepthBounds;
    bool				stencilTestEnable;
    StencilOpState		front;
    StencilOpState		back;
} DepthStencilState;

typedef struct TargetBlendState
{
    bool			blendEnable;
    BlendFactor	srcColorBlendFactor;
    BlendFactor	dstColorBlendFactor;
    BlendOp		colorBlendOp;
    BlendFactor	srcAlphaBlendFactor;
    BlendFactor	dstAlphaBlendFactor;
    BlendOp		alphaBlendOp;
} TargetBlendState;

typedef struct BlendState
{
    TargetBlendState	renderTargetBlendStates[8];
    bool				logicOpEnable;
    LogicOp			    logicOp;
} BlendState;

typedef enum PrimitiveTopology
{
    PRIMITIVE_TOPOLOGY_POINT,
    PRIMITIVE_TOPOLOGY_LINE,
    PRIMITIVE_TOPOLOGY_TRIANGLE,
    PRIMITIVE_TOPOLOGY_PATCH,
    MAX_PRIMITIVE_TOPOLOGY
} PrimitiveTopology;

typedef enum PipelineType
{
    PIPELINE_TYPE_GRAPHICS,
    PIPELINE_TYPE_COMPUTE,
    MAX_PIPELINE_TYPE
} PipelineType;

typedef struct PushConstantRange
{
    ShaderStageFlags	  stageFlags;
    uint32_t              offset;
    uint32_t              size;
} PushConstantRange;


typedef struct GraphicsPipelineCreateDesc
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

    IRootSignature* pRootSignature;
    IShader* pShader;
    uint32_t numRenderTargets;
    TextureFormat renderTargetFormats[8];
    BlendState* pBlendState;
    TextureFormat	depthStencilFormat;
    RasterizerState* pRasterizerState;
    DepthStencilState* pDepthStencilState;
    uint32_t numPushConstantRanges;
    PushConstantRange* pPushConstantRanges;
    uint32_t sampleCount;
    uint32_t sampleQuality;
    PrimitiveTopology primitiveTopology;
} GraphicsPipelineCreateDesc;


typedef struct ComputePipelineCreateDesc
{
    IRootSignature* pRootSignature;
    IShader* pShader;
} ComputePipelineCreateDesc;

struct IPipeline {};
