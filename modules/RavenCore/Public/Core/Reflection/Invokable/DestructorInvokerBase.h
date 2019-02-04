#pragma once
#include "InvokableConfig.h"

namespace Raven
{
	namespace Meta
	{
		class RVariant;

		class RDestructorInvokerBase
		{
		public:
			virtual ~RDestructorInvokerBase(void) { }

			virtual void Invoke(const RVariant &obj) = 0;
		};
	}
}