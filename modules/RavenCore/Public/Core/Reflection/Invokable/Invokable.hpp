#pragma once

#include "Core/Reflection/Type/Type.h"
#include "TypeUnpacker.hpp"

namespace std
{
	template<>
	struct hash<Raven::Meta::RInvokableSignature>
	{
		size_t operator()(
			const Raven::Meta::RInvokableSignature &signature
			) const
		{
			hash<Raven::Meta::FTypeID> hasher;

			size_t seed = 0;

			for (auto &type : signature)
				seed ^= hasher(type.GetID()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

			return seed;
		}
	};
}

namespace Raven
{
	namespace Meta
	{
		template<typename ...Types>
		RInvokableSignature RInvokable::CreateSignature(void)
		{
			static RInvokableSignature signature;
			static auto initial = true;

			if (initial)
			{
				FTypeUnpacker<Types...>::Apply(signature);

				initial = false;
			}

			return signature;
		}
	}
}
