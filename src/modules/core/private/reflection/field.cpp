#include "core/reflection/field/field.h"
#include "core/reflection/invokable/method.h"

namespace Raven
{
	namespace Meta
	{
		RField::RField(void)
			: _type(RType::Invalid())
			, _classType(RType::Invalid())
			, _name("INVALID")
			, _getter(nullptr)
			, _setter(nullptr) { }

		RField::RField(
			const std::string &name,
			RType type,
			RType classType,
			RFieldGetterBase *getter,
			RFieldSetterBase *setter
		)
			: _type(type)
			, _classType(classType)
			, _name(name)
			, _getter(getter)
			, _setter(setter) { }

		const RField &RField::Invalid(void)
		{
			static RField field;

			return field;
		}

		bool RField::SetValue(RVariant& instance, const RVariant& value, const RMethod& setter)
		{
			// read only?
			if (!instance.IsConst())
			{
				setter.Invoke(instance, value);

				return true;
			}

			return false;
		}

		bool RField::IsValid(void) const
		{
			return _getter != nullptr;
		}

		bool RField::IsReadOnly(void) const
		{
			return _setter == nullptr;
		}

		RType RField::GetType(void) const
		{
			return _type;
		}

		RType RField::GetClassType(void) const
		{
			return _classType;
		}

		const std::string &RField::GetName(void) const
		{
			return _name;
		}

		RVariant RField::GetValue(const RVariant &instance) const
		{
			return _getter->GetValue(instance);
		}

		RVariant RField::GetValueReference(const RVariant &instance) const
		{
			return _getter->GetValueReference(instance);
		}

		bool RField::SetValue(RVariant& instance, const RVariant& value) const
		{
			if (_setter && !instance.IsConst())
			{
				_setter->SetValue(instance, value);

				return true;
			}

			return false;
		}
	}
}
