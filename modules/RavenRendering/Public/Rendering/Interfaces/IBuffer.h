#pragma once
#include "Rendering/Interfaces/SharedDefines.h"

namespace Raven
{

	typedef enum FBufferUsageFlags
	{
		BUFFER_USAGE_TRANSFER_SRC = 1 << 0,
		BUFFER_USAGE_TRANSFER_DST = 1 << 1,
		BUFFER_USAGE_UNIFORM_TEXEL_BUFFER = 1 << 2,
		BUFFER_USAGE_STORAGE_TEXEL_BUFFER = 1 << 3,
		BUFFER_USAGE_UNIFORM_BUFFER = 1 << 4,
		BUFFER_USAGE_STORAGE_BUFFER = 1 << 5,
		BUFFER_USAGE_INDEX_BUFFER = 1 << 6,
		BUFFER_USAGE_VERTEX_BUFFER = 1 << 7,
		BUFFER_USAGE_INDIRECT_BUFFER = 1 << 8,
		MAX_BUFFER_TARGET_USAGE = 9
	} FBufferUsageFlags;
	DECL_ENUM_AS_FLAGS(FBufferUsageFlags)


	/*! \struct FBufferCreateDesc
	\brief Structure describing the parameters for buffer creation. */
	struct FBufferCreateDesc
	{
		FMemoryPropertyFlags	memoryProperties;
		FBufferUsageFlags		usage;
		size_t					size;
		void*					data;
	};

	/*! \struct IBuffer
	\brief Interface for a buffer. */
	struct IBuffer {};
}