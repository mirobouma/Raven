#include "Core/Reflection/Field/Variable.h"
#include "Core/Reflection/Type/Type.h"

namespace Raven
{
	namespace Meta
	{
		RVariable::RVariable(void)
			: _type(RType::Invalid())
			, _parentType(RType::Invalid())
			, _name("INVALID") { }

		RVariable::RVariable(
			const std::string &name,
			RType type,
			RVariableGetterBase *getter,
			RVariableSetterBase *setter,
			RType parentType
		)
			: _type(type)
			, _parentType(parentType)
			, _name(name)
			, _getter(getter)
			, _setter(setter) { }

		const RVariable &RVariable::Invalid(void)
		{
			static RVariable var;
			return var;
		}

		bool RVariable::IsValid(void) const
		{
			return _getter != nullptr;
		}

		bool RVariable::IsReadOnly(void) const
		{
			return _setter == nullptr;
		}

		RType RVariable::GetType() const
		{
			return _type;
		}

		RType RVariable::GetParentType(void) const
		{
			return _parentType;
		}

		const std::string &RVariable::GetName(void) const
		{
			return _name;
		}

		RVariant RVariable::GetValue(void) const
		{
			return _getter->GetValue();
		}

		bool RVariable::SetValue(const RArgument &value) const
		{
			// read only?
			if (_setter != nullptr)
			{
				_setter->SetValue(value);

				return true;
			}

			return false;
		}
	}
}
