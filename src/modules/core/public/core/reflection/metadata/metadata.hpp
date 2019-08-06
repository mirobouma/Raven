#pragma once
#include "core/reflection/type/type.h"

namespace Raven
{
	namespace Meta
	{
		template<typename PropertyType>
		const PropertyType *RMetaData::GetProperty(void) const
		{
			static_assert(std::is_base_of<IMetaProperty, PropertyType>::value,
				"Type must be a IMetaProperty."
				);

			static const auto type = typeof(PropertyType);

			auto search = _properties.find(type);

			if (search == _properties.end())
				return nullptr;

			return static_cast<const PropertyType*>(search->second);
		}
	}
}
