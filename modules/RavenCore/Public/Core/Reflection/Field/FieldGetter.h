#pragma once

#include "FieldGetterBase.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType, typename FieldType, bool IsMethod>
		class RFieldGetter : public RFieldGetterBase
		{
		public:
			RVariant GetValue(const RVariant &obj) override;
			RVariant GetValueReference(const RVariant &obj) override;
		};
	}
}

#include "FieldGetter.hpp"