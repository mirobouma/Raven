#pragma once
#include <cinttypes>
#include <cstddef>

#ifdef PLATFORM_WINDOWS
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE
#endif

namespace Raven 
{
	namespace Memory 
	{
		template <typename T> FORCEINLINE T AlignUpWithMask(T value, size_t mask)
		{
			return (T)(((size_t)value + mask) & ~mask);
		}

		template <typename T> FORCEINLINE T AlignDownWithMask(T value, size_t mask)
		{
			return (T)((size_t)value & ~mask);
		}

		template <typename T> FORCEINLINE T AlignUp(T value, size_t alignment)
		{
			return AlignUpWithMask(value, alignment - 1);
		}

		template <typename T> FORCEINLINE T AlignDown(T value, size_t alignment)
		{
			return AlignDownWithMask(value, alignment - 1);
		}

		template <typename T> FORCEINLINE bool IsAligned(T value, size_t alignment)
		{
			return 0 == ((size_t)value & (alignment - 1));
		}

		template <typename T> FORCEINLINE T DivideByMultiple(T value, size_t alignment)
		{
			return (T)((value + alignment - 1) / alignment);
		}


		//from http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
		constexpr FORCEINLINE uint32_t AlignUpToNextPowerOfTwo(uint32_t value)
		{
			value--;
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			value++;
			return value;
		}

		void* AlignedMalloc(size_t size, size_t alignment);
		void AlignedFree(void* pointer);
	}
}