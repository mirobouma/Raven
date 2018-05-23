#pragma once
#include "Math/Transform.h"

namespace Raven
{
	class RCamera
	{
		void setOrthoProjection(float width, float height, float near, float far)
		{
			mProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, near, far);
			mProjectionMatrix[1][1] *= -1;
		}

		void setPerspectiveProjection(float fov, float reverseAspect, float near, float far)
		{
			mProjectionMatrix = glm::perspective(glm::radians(fov), reverseAspect, near, far);
			mProjectionMatrix[1][1] *= -1;
		}

		const FMatrix4& getProjectionMatrix()
		{
			return mProjectionMatrix;
		}

	protected:
		FMatrix4 mProjectionMatrix;
		FTransform* mTransform;
	};
}