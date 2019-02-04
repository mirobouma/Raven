#pragma once

#include "InvokableConfig.h"
#include "ArgumentConfig.h"

namespace Raven
{
	namespace Meta
	{
		class RVariant;

		class RMethodInvokerBase
		{
		public:
			virtual ~RMethodInvokerBase(void) { }
			virtual RVariant Invoke(RVariant &obj, const ArgumentList &arguments) = 0;
		};
	}
}