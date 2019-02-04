#pragma once

#include "VariableGetterBase.h"

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

#include "VariableGetter.hpp"