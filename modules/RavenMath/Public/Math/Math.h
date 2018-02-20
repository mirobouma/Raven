#include "glm\glm.hpp"
/*!
\file Math.h
\details Math wrapper around internal math library (GLM)
*/
namespace Raven {
	namespace Math {
		// Vectors
		using FVector4 = glm::vec4;
		using FVector3 = glm::vec3;
		using FVector2 = glm::vec2;

		// Matrices
		using FMatrix = glm::mat4;
		using FMatrix4 = glm::mat4;
		using FMatrix3 = glm::mat3;
		using FMatrix2 = glm::mat2;

	}
}