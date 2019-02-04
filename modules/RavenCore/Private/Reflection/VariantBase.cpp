#include "Core/Reflection/Variant/VariantBase.h"
#include "Core/Reflection/Variant/Wrappers/ArrayWrapper.h"

namespace Raven
{
	namespace Meta
	{
		bool RVariantBase::IsArray() const
		{
			return false;
		}

		RArrayWrapper RVariantBase::GetArray() const
		{
			// invalid wrapper
			return { };
		}
	}
}