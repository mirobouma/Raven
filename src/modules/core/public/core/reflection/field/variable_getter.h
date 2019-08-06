#pragma once

#include "variable_getter_base.h"

namespace Raven
{
	namespace Meta
	{
		template<typename GlobalType, bool IsMethod>
		class RVariableGetter : public RVariableGetterBase
		{
		public:
			RVariant GetValue() override;
		};
	}
}

#include "variable_getter.hpp"