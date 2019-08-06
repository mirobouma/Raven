#pragma once

#include "core/reflection/metadata/meta_type.h"
#include "core/reflection/variant/variant.h"
#include "variable_getter.h"
#include "variable_setter.h"

namespace Raven
{
	namespace Meta
	{
		class RVariable : public IMetaType
		{
		public:
			RVariable(void);

			RVariable(
				const std::string &name,
				RType type,
				RVariableGetterBase* getter,
				RVariableSetterBase* setter,
				RType parentType = RType::Invalid()
			);

			static const RVariable &Invalid(void);

			bool IsValid(void) const;
			bool IsReadOnly(void) const;

			RType GetType(void) const;
			RType GetParentType(void) const;

			const std::string &GetName(void) const;

			RVariant GetValue(void) const;
			bool SetValue(const RArgument &value) const;

		private:
			friend struct TypeData;

			RType _type;
			RType _parentType;
			std::string _name;

			std::shared_ptr<RVariableGetterBase> _getter;
			std::shared_ptr<RVariableSetterBase> _setter;

		};
	}
}