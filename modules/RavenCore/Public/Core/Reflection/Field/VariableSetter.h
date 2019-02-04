#pragma once

#include "VariableSetterBase.h"

namespace Raven
{
	namespace Meta
	{
		template<typename GlobalType, bool IsMethod>
		class RVariableSetter : public RVariableSetterBase
		{
		public:
			void SetValue(const RArgument &value) override;
		};
	}
}

#include "VariableSetter.hpp"