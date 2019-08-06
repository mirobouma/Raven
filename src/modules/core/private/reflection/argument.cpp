#include "core/reflection/invokable/argument.h"
#include "core/reflection/type/type.h"
#include "core/reflection/variant/variant.h"

namespace Raven
{
	namespace Meta
	{
		RArgument::RArgument(void)
			: _typeID(InvalidTypeID)
			, _isArray(false)
			, _data(nullptr) { }

		RArgument::RArgument(const RArgument &rhs)
			: _typeID(rhs._typeID)
			, _isArray(rhs._isArray)
			, _data(rhs._data) { }

		RArgument::RArgument(RVariant &obj)
			: _typeID(obj.GetType().GetID())
			, _isArray(obj.GetType().IsArray())
			, _data(obj.GetPtr()) { }

		RArgument::RArgument(const RVariant &obj)
			: _typeID(obj.GetType().GetID())
			, _isArray(obj.GetType().IsArray())
			, _data(obj.GetPtr()) { }

		RArgument &RArgument::operator=(const RArgument &rhs)
		{
			_data = rhs._data;

			const_cast<FTypeID&>(_typeID) = rhs._typeID;

			return *this;
		}

		RType RArgument::GetType(void) const
		{
			return RType(_typeID, _isArray);
		}

		void *RArgument::GetPtr(void) const
		{
			return const_cast<void *>(_data);
		}
	}
}
