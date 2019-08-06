#include "core/reflection/variant/variant_base.h"
#include "core/reflection/variant/wrappers/array_wrapper.h"

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