#pragma once

#include "destructor_invoker_base.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType>
		class RDestructorInvoker : public RDestructorInvokerBase
		{
		public:
			void Invoke(const RVariant &obj) override;
		};
	}
}

#include "destructor_invoker.hpp"