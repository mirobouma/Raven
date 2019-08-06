#pragma once

#include <type_traits>
#include "type.h"
#include "type_id.h"

#if defined(__clang__)

#define COMPILER_CLANG

#elif defined(__GNUC__) || defined(__GNUG__)

#define COMPILER_GNU

#elif defined(_MSC_VER) && !__INTEL_COMPILER

#define COMPILER_MSVC

#endif

#if defined(COMPILER_CLANG) || defined(COMPILER_GNU)

#define IsTriviallyDefaultConstructible(x) std::has_trivial_default_constructor<x>::value

#else

#define IsTriviallyDefaultConstructible(x) std::is_trivially_default_constructible<x>::value

#endif

namespace Raven
{
	namespace Meta
	{
		template<typename T>
		class RTypeRegistrant
		{
		public:
			static bool bDefined;
			static void Register(FTypeID id, RTypeData* data, bool beingDefined);

		private:
			template<typename U = T>
			static void addDefaultConstructor(
				RTypeData &data,
				typename std::enable_if<
				!IsTriviallyDefaultConstructible(U)
				>::type* = nullptr
			);

			template<typename U = T>
			static void addDefaultConstructor(
				RTypeData &data,
				typename std::enable_if<
				IsTriviallyDefaultConstructible(U)
				>::type* = nullptr
			);

			template<typename U = T>
			static void applyTrivialAttributes(RTypeData &data,
				typename std::enable_if<
				!std::is_trivial<U>::value
				>::type* = nullptr
			);

			template<typename U = T>
			static void applyTrivialAttributes(RTypeData &data,
				typename std::enable_if<
				std::is_trivial<U>::value
				>::type* = nullptr
			);
		};
	}
}

#include "type_registrant.hpp"

#undef COMPILER_MSVC
#undef COMPILER_CLANG
#undef COMPILER_GNU
