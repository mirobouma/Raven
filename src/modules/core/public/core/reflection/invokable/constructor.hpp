#pragma once

#include "core/reflection/variant/variant.h"
#include "argument.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ...Args>
		RVariant RConstructor::Invoke(Args &&...args) const
		{
			ArgumentList arguments{ std::forward<Args>(args)... };

			return InvokeVariadic(arguments);
		}
	}
}
