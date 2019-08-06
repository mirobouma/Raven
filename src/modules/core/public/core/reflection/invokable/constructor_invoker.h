#pragma once

#include "constructor_invoker_base.h"

namespace Raven
{
	namespace Meta
	{
		class RVariant;
		class RArgument;

		template<typename ClassType, bool IsDynamic, bool IsObjectWrapped, typename ...ArgTypes>
		class RConstructorInvoker : public RConstructorInvokerBase
		{
		public:
			static_assert(THIS_ARG_COUNT <= MaxArgumentCount,
				"Constructor has too many arguments. It's time to generate more overloads."
				);

			RVariant Invoke(const ArgumentList &arguments) override;
		};
	}
}

#include "constructor_invoker.hpp"
