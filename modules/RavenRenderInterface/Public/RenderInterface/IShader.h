#pragma once
#include <string>

namespace Raven
{
	/*! \enum EShaderStage
	 \brief Enumeration describing the stage of the shader an FShaderProperty is.
	 \sa FShaderProperty
	 */
	typedef enum EShaderStage 
	{
		SHADER_STAGE_VERTEX,
		SHADER_STAGE_TESSELLATION_CONTROL,
		SHADER_STAGE_TESSELLATION_EVALUATION,
		SHADER_STAGE_GEOMETRY,
		SHADER_STAGE_FRAGMENT,
		SHADER_STAGE_COMPUTE,
		MAX_SHADER_STAGE
	} EShaderStage;

	/*! \enum FShaderDefinition
	\brief structure describing a shader definition.
	\sa FShaderProperty
	*/
	typedef struct FShaderDefinition
	{
		std::string name;
		std::string value;
	} FShaderDefinition;

	/*! \struct FShaderProperty
	 \brief Structure describing a shader to load.
	 */
	typedef struct FShaderProperty
	{
		EShaderStage stage;
		std::string path;
	} FShaderProperty;

	/*! \struct FShaderCreateDesc
	 \brief Structure describing the parameters for shader creation.
	 */
	typedef struct FShaderCreateDesc
	{
		FShaderProperty shaders[MAX_SHADER_STAGE];
		std::vector<FShaderDefinition> definitions;
	} FShaderCreateDesc;

	/*! \struct IShader
	 \brief Interface for a shader.
	 */
	struct IShader {};
}