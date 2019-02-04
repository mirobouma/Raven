#include "Core/Reflection/Enum/Enum.h"
#include "Core/Reflection/Variant/Variant.h"

namespace Raven
{
	namespace Meta
	{
		REnum::REnum(const REnumBase *base)
			: _base(base) { }

		bool REnum::IsValid(void) const
		{
			return _base != nullptr;
		}

		REnum::operator bool(void) const
		{
			return _base != nullptr;
		}

		bool REnum::operator==(const REnum &rhs) const
		{
			return _base == rhs._base;
		}

		bool REnum::operator!=(const REnum &rhs) const
		{
			return _base != rhs._base;
		}

		std::string REnum::GetName(void) const
		{
			return _base ? _base->GetName() : std::string();
		}

		RType REnum::GetType(void) const
		{
			return _base ? _base->GetType() : RType::Invalid();
		}

		RType REnum::GetParentType(void) const
		{
			return _base ? _base->GetParentType() : RType::Invalid();
		}

		RType REnum::GetUnderlyingType(void) const
		{
			return _base ? _base->GetUnderlyingType() : RType::Invalid();
		}

		std::vector<std::string> REnum::GetKeys(void) const
		{
			return _base ? _base->GetKeys() : std::vector<std::string>();
		}

		std::vector<RVariant> REnum::GetValues(void) const
		{
			return _base ? _base->GetValues() : std::vector<RVariant>();
		}

		std::string REnum::GetKey(const RArgument &value) const
		{
			return _base ? _base->GetKey(value) : std::string();
		}

		RVariant REnum::GetValue(const std::string &key) const
		{
			return _base ? _base->GetValue(key) : RVariant();
		}
	}
}
