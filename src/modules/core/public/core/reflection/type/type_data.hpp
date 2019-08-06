#pragma once

#include "Core/Reflection/ReflectionDatabase.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType, bool IsDynamic, bool IsWrapped, typename ...Args>
		void RTypeData::AddConstructor(
			const RMetaData::Initializer &meta
		)
		{
			RInvokableSignature signature =
				Invokable::CreateSignature<Args...>();

			Constructor ctor{
				typeof(ClassType),
				signature,
				new ConstructorInvoker<ClassType, IsDynamic, IsWrapped, Args...>(),
				IsDynamic
			};

			ctor.m_meta = meta;

			if (IsDynamic)
				dynamicConstructors.emplace(signature, ctor);
			else
				constructors.emplace(signature, ctor);
		}



		template<typename ClassType>
		void RTypeData::SetArrayConstructor(void)
		{
			arrayConstructor = Constructor{
				typeof(ClassType),
				{ },
				new ConstructorInvoker<Array<ClassType>, false, false>(),
				false
			};
		}

		template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
		void RTypeData::AddField(
			const std::string &name,
			GetterReturnType(ClassType::*methodGetter)(void),
			void (ClassType::*methodSetter)(SetterArgumentType),
			const RMetaData::Initializer &meta
		)
		{
			typedef RFieldGetter<ClassType, GetterReturnType, true> GetterType;
			typedef RFieldSetter<ClassType, SetterArgumentType, true> SetterType;

			static_assert(std::is_same<typename std::decay<GetterReturnType>::type, typename std::decay<FieldType>::type>::value,
				"Return type of getter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			static_assert(std::is_same<typename std::decay<SetterArgumentType>::type, typename std::decay<FieldType>::type>::value,
				"Argument type of setter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!methodGetter ? nullptr : new GetterType(methodGetter),
				!methodSetter ? nullptr : new SetterType(methodSetter)
			);

			fields.back().m_meta = meta;
		}



		template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
		void RTypeData::AddField(
			const std::string &name,
			GetterReturnType(ClassType::*methodGetter)(void) const,
			void (ClassType::*methodSetter)(SetterArgumentType),
			const RMetaData::Initializer &meta
		)
		{
			typedef RFieldGetter<ClassType, GetterReturnType, true> GetterType;
			typedef RFieldSetter<ClassType, SetterArgumentType, true> SetterType;

			static_assert(std::is_same<typename std::decay<GetterReturnType>::type, typename std::decay<FieldType>::type>::value,
				"Return type of getter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			static_assert(std::is_same<typename std::decay<SetterArgumentType>::type, typename std::decay<FieldType>::type>::value,
				"Argument type of setter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!methodGetter ? nullptr : new GetterType(methodGetter),
				!methodSetter ? nullptr : new SetterType(methodSetter)
			);

			fields.back().m_meta = meta;
		}

		template<typename ClassType, typename FieldType, typename GetterReturnType>
		void RTypeData::AddField(
			const std::string &name,
			GetterReturnType(ClassType::*methodGetter)(void),
			typename RFieldSetter<ClassType, FieldType, false>::Signature RFieldSetter,
			const RMetaData::Initializer &meta
		)
		{
			typedef RFieldGetter<ClassType, GetterReturnType, true> GetterType;

			static_assert(std::is_same<typename std::decay<GetterReturnType>::type, typename std::decay<FieldType>::type>::value,
				"Return type of getter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!methodGetter ? nullptr : new GetterType(methodGetter),
				!RFieldSetter ? nullptr : new RFieldSetter<ClassType, FieldType, false>(RFieldSetter)
			);

			fields.back().m_meta = meta;
		}



		template<typename ClassType, typename FieldType, typename GetterReturnType>
		void RTypeData::AddField(
			const std::string &name,
			GetterReturnType(ClassType::*methodGetter)(void) const,
			typename RFieldSetter<ClassType, FieldType, false>::Signature RFieldSetter,
			const RMetaData::Initializer &meta
		)
		{
			typedef RFieldGetter<ClassType, GetterReturnType, true> GetterType;

			static_assert(std::is_same<typename std::decay<GetterReturnType>::type, typename std::decay<FieldType>::type>::value,
				"Return type of getter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!methodGetter ? nullptr : new GetterType(methodGetter),
				!RFieldSetter ? nullptr : new RFieldSetter<ClassType, FieldType, false>(RFieldSetter)
			);

			fields.back().m_meta = meta;
		}



		template<typename ClassType, typename FieldType, typename SetterArgumentType>
		void RTypeData::AddField(
			const std::string &name,
			typename RFieldGetter<ClassType, FieldType, false>::Signature RFieldGetter,
			void (ClassType::*methodSetter)(SetterArgumentType),
			const RMetaData::Initializer &meta
		)
		{
			typedef RFieldSetter<ClassType, SetterArgumentType, true> SetterType;

			static_assert(std::is_same<typename std::decay<SetterArgumentType>::type, typename std::decay<FieldType>::type>::value,
				"Argument type of setter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!RFieldGetter ? nullptr : new RFieldGetter<ClassType, FieldType, false>(RFieldGetter),
				!methodSetter ? nullptr : new SetterType(methodSetter)
			);

			fields.back().m_meta = meta;
		}



		template<typename ClassType, typename FieldType>
		void RTypeData::AddField(
			const std::string &name,
			typename RFieldGetter<ClassType, FieldType, false>::Signature RFieldGetter,
			typename RFieldSetter<ClassType, FieldType, false>::Signature RFieldSetter,
			const RMetaData::Initializer &meta
		)
		{
			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!RFieldGetter ? nullptr : new RFieldGetter<ClassType, FieldType, false>(RFieldGetter),
				!RFieldSetter ? nullptr : new RFieldSetter<ClassType, FieldType, false>(RFieldSetter)
			);

			fields.back().m_meta = meta;
		}



		template<typename ClassType, typename FieldType, typename GetterType, typename SetterType>
		void RTypeData::AddStaticField(const std::string &name, GetterType getter, SetterType setter, const RMetaData::Initializer &meta)
		{
			typedef GlobalGetter<FieldType, true> GlobalGetterType;
			typedef GlobalSetter<FieldType, true> GlobalSetterType;

			staticFields.emplace_back(
				name,
				typeof(FieldType),
				!getter ? nullptr : new GlobalGetterType(
					reinterpret_cast<typename GlobalGetterType::Signature>(getter)
				),
				!setter ? nullptr : new GlobalSetterType(
					reinterpret_cast<typename GlobalSetterType::Signature>(setter)
				),
				typeof(ClassType)
			);

			staticFields.back().m_meta = meta;
		}



		template<typename ClassType, typename FieldType, typename GetterType>
		void RTypeData::AddStaticField(const std::string &name, GetterType getter, FieldType *RFieldSetter, const RMetaData::Initializer &meta)
		{
			typedef GlobalGetter<FieldType, true> GlobalGetterType;
			typedef GlobalSetter<FieldType, false> GlobalSetterType;

			staticFields.emplace_back(
				name,
				typeof(FieldType),
				!getter ? nullptr : new GlobalGetterType(
					reinterpret_cast<typename GlobalGetterType::Signature>(getter)
				),
				!RFieldSetter ? nullptr : new GlobalSetterType(RFieldSetter),
				typeof(ClassType)
			);

			staticFields.back().m_meta = meta;
		}



		template<typename ClassType, typename FieldType, typename SetterType>
		void RTypeData::AddStaticField(const std::string &name, FieldType *RFieldGetter, SetterType setter, const RMetaData::Initializer &meta)
		{
			typedef GlobalGetter<FieldType, false> GlobalGetterType;
			typedef GlobalSetter<FieldType, true> GlobalSetterType;

			staticFields.emplace_back(
				name,
				typeof(FieldType),
				!RFieldGetter ? nullptr : new GlobalGetterType(RFieldGetter),
				!setter ? nullptr : new GlobalSetterType(
					reinterpret_cast<typename GlobalSetterType::Signature>(setter)
				),
				typeof(ClassType)
			);

			staticFields.back().m_meta = meta;
		}



		template<typename ClassType, typename FieldType>
		void RTypeData::AddStaticField(const std::string &name, FieldType *RFieldGetter, FieldType *RFieldSetter, const RMetaData::Initializer &meta)
		{
			typedef GlobalGetter<FieldType, false> GlobalGetterType;
			typedef GlobalSetter<FieldType, false> GlobalSetterType;

			staticFields.emplace_back(
				name,
				typeof(FieldType),
				!RFieldGetter ? nullptr : new GlobalGetterType(RFieldGetter),
				!RFieldSetter ? nullptr : new GlobalSetterType(RFieldSetter),
				typeof(ClassType)
			);

			staticFields.back().m_meta = meta;
		}



		template<typename ClassType>
		void RTypeData::SetDestructor(void)
		{
			destructor = {
				typeof(ClassType),
				new DestructorInvoker<ClassType>()
			};
		}



		template<typename MethodType>
		void RTypeData::AddMethod(
			const std::string &name,
			MethodType method,
			const RMetaData::Initializer &meta
		)
		{
			Method meth(name, method);

			meth.m_meta = meta;

			methods[name].emplace(meth.GetSignature(), meth);
		}



		template<typename ClassType, typename FunctionType>
		void RTypeData::AddStaticMethod(
			const std::string &name,
			FunctionType function,
			const RMetaData::Initializer &meta
		)
		{
			Function fn(name, function, typeof(ClassType));

			fn.m_meta = meta;

			staticMethods[name].emplace(fn.GetSignature(), fn);
		}



		template<typename EnumType>
		void RTypeData::SetEnum(
			const std::string &name,
			const typename REnumContainer<EnumType>::Initializer &initializer
		)
		{
			enumeration = new REnumContainer<EnumType>(name, initializer);
		}
	}
}
