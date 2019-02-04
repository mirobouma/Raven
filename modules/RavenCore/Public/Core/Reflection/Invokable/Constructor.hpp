#pragma once

#include "Core/Reflection/Variant/Variant.h"
#include "Argument.h"

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
