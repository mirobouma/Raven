#include "core/reflection/variant/wrappers/array_wrapper.h"
#include "core/reflection/variant/variant.h"
#include "core/debug/log_manager.h"

namespace Raven
{
	namespace Meta
	{
		RArrayWrapper::RArrayWrapper(void)
			: _isConst(true)
			, _base(nullptr) { }

		RVariant RArrayWrapper::GetValue(size_t index) const
		{
			return _base ? _base->GetValue(index) : RVariant();
		}

		void RArrayWrapper::SetValue(size_t index, const RArgument &value)
		{ 
			RASSERT_MSG(!_isConst, "Array is not const");

			if (_base)
				_base->SetValue(index, value);
		}

		void RArrayWrapper::Insert(size_t index, const RArgument &value)
		{
			RASSERT_MSG(!_isConst, "Array is const.");

			if (_base)
				_base->Insert(index, value);
		}

		void RArrayWrapper::Remove(size_t index)
		{
			RASSERT_MSG(!_isConst, "Array is const.");

			if (_base)
				_base->Remove(index);
		}

		size_t RArrayWrapper::Size(void) const
		{
			return _base ? _base->Size() : 0;
		}

		bool RArrayWrapper::IsValid(void) const
		{
			return _base != nullptr;
		}

		bool RArrayWrapper::IsConst(void) const
		{
			return _isConst;
		}
	}
}