#pragma once

#include <vector>
#include <unordered_map>

#define THIS_ARG_COUNT sizeof...( ArgTypes )

namespace Raven
{
	namespace Meta
	{
		class RType;

		typedef std::vector<RType> RInvokableSignature;

		template<typename T>
		using InvokableOverloadMap =
			std::unordered_multimap<RInvokableSignature, T>;

		// maximum number of arguments supported
		const size_t MaxArgumentCount = 10;
	}
}