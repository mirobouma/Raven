#include "core/reflection/enum/enum_base.h"
#include "core/reflection/variant/variant.h"

namespace Raven
{
	namespace Meta
	{
		REnumBase::REnumBase(const std::string &name, FTypeID owner)
			: _parentType(owner)
			, _name(name) { }

		RType REnumBase::GetParentType(void) const
		{
			return _parentType;
		}

		const std::string &REnumBase::GetName(void) const
		{
			return _name;
		}
	}
}