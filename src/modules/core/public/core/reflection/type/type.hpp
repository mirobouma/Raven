#pragma once

#include <functional>
#include "core/reflection/variant/variant.h"

namespace std
{
	template<>
	struct hash<Raven::Meta::RType>
	{
		size_t operator()(const Raven::Meta::RType &type) const
		{
			return hash<Raven::Meta::FTypeID>()(type.GetID());
		}
	};
}

namespace Raven
{
	namespace Meta
	{
		template<typename T>
		RType RType::Get(T &&obj)
		{
			return { typeof(T) };
		}

		template<typename T>
		bool RType::DerivesFrom(void) const
		{
			return DerivesFrom(typeof(T));
		}
	}
}
