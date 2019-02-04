#pragma once

#include "InvokableConfig.h"
#include "ArgumentConfig.h"

namespace Raven
{
	namespace Meta
	{
		class RVariant;
		class RArgument;

		class RConstructorInvokerBase
		{
		public:
			virtual ~RConstructorInvokerBase(void) { }

			virtual RVariant Invoke(const ArgumentList &arguments) = 0;
		};
	}
}