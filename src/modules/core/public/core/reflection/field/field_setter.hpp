#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType, typename ArgumentType>
		class RFieldSetter<ClassType, ArgumentType, true> : public RFieldSetterBase
		{
		public:
			typedef typename std::remove_reference<ArgumentType>::type NonReferenceArgType;
			typedef void (ClassType::*RSignature)(ArgumentType);
			typedef void (ClassType::*RSignatureConst)(ArgumentType) const;

			RFieldSetter(RSignature method)
				: _method(method) { }

			RFieldSetter(RSignatureConst method)
				: _method(reinterpret_cast<RSignature>(method)) { }

			void SetValue(RVariant &obj, const RVariant &value) override
			{
				auto &instance = obj.GetValue<ClassType>();
				(instance.*_method)(value.GetValue<NonReferenceArgType>());
			}

		private:
			RSignature _method;
		};

		template<typename ClassType, typename FieldType>
		class RFieldSetter<ClassType, FieldType, false> : public RFieldSetterBase
		{
		public:
			typedef FieldType(ClassType::*RSignature);
			typedef typename std::remove_const<FieldType>::type NonConstFieldType;

			RFieldSetter(RSignature field)
				: _field(field) { }

			void SetValue(RVariant &obj, const RVariant &value) override
			{
				auto &instance = obj.GetValue<ClassType>();
				const_cast<NonConstFieldType&>(instance.*_field) = value.GetValue<FieldType>();
			}

		private:
			RSignature _field;
		};
	}
}