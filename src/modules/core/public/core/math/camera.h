#pragma once
#include "core/math/math.h"
#include "core/math/transform.h"

namespace Raven
{
	class RCamera
	{
	public:
		
		const FMatrix4& getProjectionMatrix()
		{
			return mProjectionMatrix;
		}

		float getNearZ()
		{
			return mNear;
		}


		float getFarZ()
		{
			return mFar;
		}

		FTransform* getTransform()
		{
			return &mTransform;
		}

		void setOrthoProjection(float width, float height, float nearZ, float farZ)
		{
			mProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearZ, farZ);

			mNear = nearZ;
			mFar = farZ;
		}

		void setPerspectiveProjection(float fov, float reverseAspect, float nearZ, float farZ)
		{
			mProjectionMatrix = glm::perspective(glm::radians(fov), reverseAspect, nearZ, farZ);

			mNear = nearZ;
			mFar = farZ;
		}

	protected:
		float mNear;
		float mFar;
		FMatrix4 mProjectionMatrix;
		FTransform mTransform;
	};
}