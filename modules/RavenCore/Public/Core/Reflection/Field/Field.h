#pragma once

#include "Core/Reflection/Metadata/MetaType.h"
#include "Core/Reflection/Invokable/Method.h"
#include "Core/Reflection/Variant/Variant.h"
#include "FieldGetter.h"
#include "FieldSetter.h"

namespace Raven
{
	namespace Meta
	{
		class RField : public IMetaType
		{
		public:
			RField();
			RField(
				const std::string &name,
				RType type,
				RType classType,
				RFieldGetterBase* getter,
				RFieldSetterBase* setter
			);

			static bool SetValue(RVariant& instance, const RVariant& value, const RMethod& setter);
			static const RField &Invalid(void);

			bool IsValid() const;
			bool IsReadOnly() const;

			RType GetType() const;
			RType GetClassType() const;

			const std::string &GetName() const;

			RVariant GetValue(const RVariant& instance) const;
			RVariant GetValueReference(const RVariant& instance) const;

			bool SetValue(RVariant& instance, const RVariant& value) const;

		private:
			friend class RTypeData;

			RType _type;
			RType _classType;

			std::string _name;

			std::shared_ptr<RFieldGetterBase> _getter;
			std::shared_ptr<RFieldSetterBase> _setter;
		};
	}
}