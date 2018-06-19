#pragma once
#include "Math.h"

namespace Raven 
{
	const float radToDeg = glm::pi<float>() / 180.0f;
	const float degToRad =  180.0f / glm::pi<float>();

	/*
	\struct FTransform
	\brief Transformation matrix.
	*/
	struct FTransform 
	{
		FTransform() : mRequireMatrixUpdate(true), mPosition(0.0f, 0.0f, 0.0f), mRotation(0.0f, 0.0f, 0.0f), mScale(1.0f, 1.0f, 1.0f) {};
			
		void translate(float x, float y, float z)
		{
			mPosition += glm::vec3(x, y, z);
			mRequireMatrixUpdate = true;
		}

		void translate(const FVector3& direction)
		{
			mPosition += direction;
			mRequireMatrixUpdate = true;
		}

		void setPosition(const  FVector3& position)
		{
			mPosition = position;
			mRequireMatrixUpdate = true;
		}

		void setXPosition(float x)
		{
			mPosition.x = x;
			mRequireMatrixUpdate = true;
		}

		void setYPosition(float y)
		{
			mPosition.y = y;
			mRequireMatrixUpdate = true;
		}

		void setZPosition(float z)
		{
			mPosition.z = z;
			mRequireMatrixUpdate = true;
		}

		void rotate(float x, float y, float z)
		{
			mRotation += glm::vec3(x, y, z);
			mRequireMatrixUpdate = true;
		}

		void rotate(const FVector3& direction)
		{
			mRotation += direction;
			mRequireMatrixUpdate = true;
		}

		void setRotation(const FVector3& rotation)
		{
			mRotation = rotation;
			mRequireMatrixUpdate = true;
		}

		void setScale(const glm::vec3 &scale)
		{
			mScale = scale;
			mRequireMatrixUpdate = true;
		}

		void lookAt(const glm::vec3& position)
		{
			if ((position - mPosition) == glm::vec3(0, 0, 0)) 
				return;

			glm::vec3 direction = glm::normalize(position - mPosition);

			mRotation.x = asinf(direction.y);
			mRotation.y = -atan2f(direction.x, direction.z);
			mRequireMatrixUpdate = true;
		}
		
		const FVector3& GetScale()
		{
			return mScale;
		}

		const FVector3& GetPosition()
		{
			return mPosition;
		}

		const FVector3& GetRotation()
		{
			return mRotation;
		}

		FMatrix4 getMatrix()
		{
			if (mRequireMatrixUpdate)
			{
				mRequireMatrixUpdate = false;
				mTransformMatrix = glm::lookAt(mPosition, mPosition + GetForward(), glm::vec3(0, 1, 0));
			}

			return mTransformMatrix;
		}

		const glm::vec3 GetUp()
		{
			glm::mat4 orientation;
			orientation = glm::rotate(mRotation.x, glm::vec3(1, 0, 0));
			orientation = glm::rotate(orientation, mRotation.y, glm::vec3(0, 1, 0));
			orientation = glm::rotate(orientation, mRotation.z, glm::vec3(0, 0, 1));

			glm::vec4 up = glm::vec4(0, 1, 0, 0) * orientation;
			return glm::normalize(glm::vec3(up));	
		}

		const glm::vec3 GetRight()
		{
			glm::mat4 orientation;
			orientation = glm::rotate(mRotation.x, glm::vec3(1, 0, 0));
			orientation = glm::rotate(orientation, mRotation.y, glm::vec3(0, 1, 0));
			orientation = glm::rotate(orientation, mRotation.z, glm::vec3(0, 0, 1));

			glm::vec4 right = glm::vec4(1, 0, 0, 0) * orientation;
			return glm::normalize(glm::vec3(right));
		}
			

		const glm::vec3 GetForward()
		{
			glm::mat4 orientation;
			orientation = glm::rotate(mRotation.x, glm::vec3(1, 0, 0));
			orientation = glm::rotate(orientation, mRotation.y, glm::vec3(0, 1, 0));
			orientation = glm::rotate(orientation, mRotation.z, glm::vec3(0, 0, 1));

			glm::vec4 forward = glm::vec4(0, 0, 1, 0) * orientation;
			return glm::normalize(glm::vec3(forward));
		}

		FVector3 mPosition, mRotation, mScale;
		glm::mat4 mTransformMatrix;

		bool mRequireMatrixUpdate;
	};
}