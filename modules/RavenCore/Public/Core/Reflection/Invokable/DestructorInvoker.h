#pragma once

#include "DestructorInvokerBase.h"

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

#include "DestructorInvoker.hpp"