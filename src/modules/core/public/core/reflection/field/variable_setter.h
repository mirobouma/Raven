#pragma once
#include "variable_setter_base.h"

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

#include "variable_setter.hpp"