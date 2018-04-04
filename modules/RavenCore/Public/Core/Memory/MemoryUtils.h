#pragma once
#include <cinttypes>

namespace Raven 
{
	namespace Memory 
	{
		template <typename T> __forceinline T AlignUpWithMask(T value, size_t mask)
		{
			return (T)(((size_t)value + mask) & ~mask);
		}

		template <typename T> __forceinline T AlignDownWithMask(T value, size_t mask)
		{
			return (T)((size_t)value & ~mask);
		}

		template <typename T> __forceinline T AlignUp(T value, size_t alignment)
		{
			return AlignUpWithMask(value, alignment - 1);
		}

		template <typename T> __forceinline T AlignDown(T value, size_t alignment)
		{
			return AlignDownWithMask(value, alignment - 1);
		}

		template <typename T> __forceinline bool IsAligned(T value, size_t alignment)
		{
			return 0 == ((size_t)value & (alignment - 1));
		}

		template <typename T> __forceinline T DivideByMultiple(T value, size_t alignment)
		{
			return (T)((value + alignment - 1) / alignment);
		}

		void* AlignedMalloc(size_t size, size_t alignment);
		void AlignedFree(void* pointer);
	}
}