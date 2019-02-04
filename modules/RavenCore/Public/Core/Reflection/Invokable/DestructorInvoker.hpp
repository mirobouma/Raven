#pragma once

#include "Core/Reflection/Variant/Variant.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType>
		void RDestructorInvoker<ClassType>::Invoke(const RVariant& obj)
		{
			auto &instance = obj.GetValue<ClassType>();
			instance.~ClassType();
		}
	}
}
