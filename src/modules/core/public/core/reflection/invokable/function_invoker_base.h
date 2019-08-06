#pragma once
#include "invokable_config.h"
#include "argument_config.h"

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