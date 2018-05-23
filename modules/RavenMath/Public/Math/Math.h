#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*!
\file Math.h
\details Math wrapper around internal math library (GLM)
*/
namespace Raven 
{
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
