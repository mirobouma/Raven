#pragma once
#include <string>
#include <vector>

namespace Raven
{
    /*! \enum EShaderStage
     \brief Enumeration describing the stage of the shader an FShaderProperty is.
     \sa FShaderProperty
     */
    typedef enum ShaderStage
    {
        SHADER_STAGE_VERTEX,
        SHADER_STAGE_TESSELLATION_CONTROL,
        SHADER_STAGE_TESSELLATION_EVALUATION,
        SHADER_STAGE_GEOMETRY,
        SHADER_STAGE_FRAGMENT,
        SHADER_STAGE_COMPUTE,
        MAX_SHADER_STAGE
    } ShaderStage;

    /*! \enum FShaderDefinition
    \brief structure describing a shader definition.
    \sa FShaderProperty
    */
    typedef struct ShaderDefinition
    {
        std::string name;
        std::string value;
    } ShaderDefinition;

    /*! \struct FShaderProperty
     \brief Structure describing a shader to load.
     */
    typedef struct ShaderModule
    {
        ShaderStage stage;
        char* shaderCode;
        uint64_t shaderCodeSize;
    } ShaderProperty;

    /*! \struct FShaderCreateDesc
     \brief Structure describing the parameters for shader creation.
     */
    typedef struct ShaderCreateDesc
    {
        ShaderProperty shaders[MAX_SHADER_STAGE];
        std::vector<ShaderDefinition> definitions;
    } ShaderCreateDesc;

    /*! \struct IShader
     \brief Interface for a shader.
     */
    struct IShader {};
}
