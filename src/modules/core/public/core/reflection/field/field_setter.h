#pragma once
#include "field_setter_base.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType, typename FieldType, bool IsMethod>
		class RFieldSetter : public RFieldSetterBase
		{
		public:
			void SetValue(RVariant &obj, const RVariant &value) override;
		};
	}
}

#include "field_setter.hpp"