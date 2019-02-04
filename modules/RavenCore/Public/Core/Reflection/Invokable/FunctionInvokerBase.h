#pragma once

#include "InvokableConfig.h"
#include "ArgumentConfig.h"

namespace Raven
{
	namespace Meta
	{
		class RVariant;

		class RFunctionInvokerBase
		{
		public:
			virtual ~RFunctionInvokerBase(void) { }

			virtual RVariant Invoke(const ArgumentList &arguments) = 0;
		};
	}
}