#include "Core/Reflection/Variant/Variant.h"
#include "Core/Reflection/Type/Type.h"

namespace Raven
{
	namespace Meta
	{
		RVariant::RVariant(void)
			: _isConst(true)
			, _data(nullptr) { }

		RVariant::RVariant(const RVariant &rhs)
			: _isConst(rhs._isConst)
			, _data(rhs._data ? rhs._data->Clone() : nullptr) { }

		RVariant::RVariant(RVariant &&rhs)
			: _isConst(rhs._isConst)
			, _data(rhs._data)
		{
			rhs._isConst = true;
			rhs._data = nullptr;
		}

		RVariant::~RVariant(void)
		{
			delete _data;
		}

		RVariant &RVariant::operator=(const RVariant &rhs)
		{
			RVariant(rhs).Swap(*this);

			return *this;
		}

		RVariant &RVariant::operator=(RVariant &&rhs)
		{
			rhs.Swap(*this);

			RVariant().Swap(rhs);

			return *this;
		}

		RVariant::operator bool(void) const
		{
			return _data != nullptr;
		}

		RType RVariant::GetType(void) const
		{
			return _data ? _data->GetType() : RType::Invalid();
		}

		RArrayWrapper RVariant::GetArray(void) const
		{
			return _data ? _data->GetArray() : RArrayWrapper();
		}

		int RVariant::ToInt(void) const
		{
			return _data ? _data->ToInt() : int();
		}

		bool RVariant::ToBool(void) const
		{
			return _data ? _data->ToBool() : bool();
		}

		float RVariant::ToFloat(void) const
		{
			return _data ? _data->ToFloat() : float();
		}

		double RVariant::ToDouble(void) const
		{
			return _data ? _data->ToDouble() : double();
		}

		std::string RVariant::ToString(void) const
		{
			return _data ? _data->ToString() : std::string();
		}

		void RVariant::Swap(RVariant &other)
		{
			std::swap(_data, other._data);
		}

		bool RVariant::IsValid(void) const
		{
			return _data != nullptr;
		}

		bool RVariant::IsConst(void) const
		{
			return _isConst;
		}

		bool RVariant::IsArray(void) const
		{
			return _data ? _data->IsArray() : false;
		}

		void *RVariant::GetPtr(void) const
		{
			return _data ? _data->GetPtr() : nullptr;
		}
	}
}
