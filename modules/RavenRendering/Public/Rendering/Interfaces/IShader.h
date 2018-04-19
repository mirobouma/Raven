#pragma once
#include <string>

namespace Raven
{
	/*! \enum EShaderType
	 \brief Enumeration describing the type of shader an FShaderProperty is.
	 \sa FShaderProperty
	 */
	typedef enum EShaderType {
		SHADER_TYPE_VERTEX,
		SHADER_TYPE_PIXEL,
		SHADER_TYPE_GEOMETRY,
		SHADER_TYPE_DOMAIN,
		SHADER_TYPE_HULL,
		MAX_SHADER_TYPE
	}EShaderType;

	/*! \struct FShaderProperty
	 \brief Structure describing a shader to load and its defines.
	 */
	typedef struct FShaderProperty
	{
		EShaderType type;
		std::string path;
		std::string defines;
	} FShaderProperty;

	/*! \struct FShaderCreateDesc
	 \brief Structure describing the parameters for shader creation.
	 */
	struct FShaderCreateDesc
	{
		uint32_t 		numShaders;
		FShaderProperty shaders[MAX_SHADER_TYPE];
	};

	/*! \struct IShader
	 \brief Interface for a shader.
	 */
	struct IShader {};
}