#include "Core/Reflection/Variant/Wrappers/ObjectWrapper.h"
#include "Core/Object/Object.h"

#include "Core/Reflection/Type/Type.h"

namespace Raven
{
	namespace Meta
	{
		RObjectWrapper::RObjectWrapper(RObject* instance)
			: _object(instance)
		{
		}

		RType RObjectWrapper::GetType(void) const
		{
			return _object->GetType();
		}

		void *RObjectWrapper::GetPtr(void) const
		{
			return _object;
		}

		int RObjectWrapper::ToInt(void) const
		{
			return int();
		}

		bool RObjectWrapper::ToBool(void) const
		{
			return bool();
		}

		float RObjectWrapper::ToFloat(void) const
		{
			return float();
		}

		double RObjectWrapper::ToDouble(void) const
		{
			return double();
		}

		std::string RObjectWrapper::ToString(void) const
		{
			return std::string();
		}

		RVariantBase* RObjectWrapper::Clone(void) const
		{
			return new RObjectWrapper(_object);
		}
	}
}