/* ----------------------------------------------------------------------------
** Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
**
** FieldGetter.hpp
** --------------------------------------------------------------------------*/

#pragma once

namespace Raven
{
	namespace Meta
	{
		// Getter for method of an instance.
		template<typename ClassType, typename ReturnType>
		class RFieldGetter<ClassType, ReturnType, true> : public RFieldGetterBase
		{
		public:
			typedef ReturnType(ClassType::*RSignature)(void);
			typedef ReturnType(ClassType::*RSignatureConst)(void) const;

			RFieldGetter(RSignature method)
				: _method(reinterpret_cast<RSignatureConst>(method)) { }

			RFieldGetter(RSignatureConst method)
				: _method(method) { }

			RVariant GetValue(const RVariant &obj) override
			{
				auto &instance = obj.GetValue<ClassType>();

				return (instance.*_method)();
			}

			RVariant GetValueReference(const RVariant &obj) override
			{
				return _GetValueReference(obj);
			}

		private:
			template<typename T = ReturnType>
			RVariant _GetValueReference(
				const RVariant &obj,
				typename std::enable_if<
				std::is_lvalue_reference<T>::value
				>::type* = nullptr
			)
			{
				auto &instance = obj.GetValue<ClassType>();
				return RVariant{ (instance.*_method)(), Variant_Policy::NoCopy() };
			}

			template<typename T = ReturnType>
			RVariant _GetValueReference(
				const RVariant &obj,
				typename std::enable_if<
				!std::is_lvalue_reference<T>::value
				>::type* = nullptr
			)
			{
				// non l-value reference return types must return by value
				return GetValue(obj);
			}

			RSignatureConst _method;
		};

		// Getter for fields that are not part of an instance.
		template<typename ClassType, typename FieldType>
		class RFieldGetter<ClassType, FieldType, false> : public RFieldGetterBase
		{
		public:
			typedef FieldType(ClassType::*RSignature);

			RFieldGetter(RSignature field)
				: _field(field) { }

			RVariant GetValue(const RVariant &obj) override
			{
				auto &instance = obj.GetValue<ClassType>();

				return instance.*_field;
			}

			RVariant GetValueReference(const RVariant &obj) override
			{
				auto &instance = obj.GetValue<ClassType>();

				return RVariant{ instance.*_field, Variant_Policy::NoCopy() };
			}

		private:
			RSignature _field;
		};
	}
}