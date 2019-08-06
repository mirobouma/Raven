#pragma once
#include <string>
#include "type_id.h"
#include <vector>
#include "type.h"
#include <set>
#include <unordered_map>
#include "core/reflection/metadata/metadata.h"
#include "core/reflection/metadata/meta_type.h"
#include "core/reflection/field/field_getter.h"
#include "core/reflection/field/field_setter.h"
#include "core/reflection/field/variable.h"
#include "core/reflection/field/field.h"
#include "core/reflection/invokable/constructor.h"
#include "core/reflection/invokable/destructor.h"
#include "core/reflection/enum/enum.h"
#include "core/reflection/enum/enum_container.h"


namespace Raven
{
	namespace Meta
	{

		/*! Class describing all of a type data. */
		class RTypeData : public IMetaType
		{
			template<typename T>
			friend class RTypeRegistrant;

			friend class std::map<FTypeID, RTypeData>;
			friend class std::tuple<FTypeID, RTypeData>;
			friend class RReflectionDatabase;
			friend class RType;

		protected:
			RTypeData(void);
			RTypeData(const std::string &name);

			void LoadBaseClasses(
				RReflectionDatabase &db,
				FTypeID thisType,
				const std::initializer_list<RType> &classes
			);

			template<typename ClassType, bool IsDynamic, bool IsWrapped, typename ...Args>
			void AddConstructor(
				const RMetaData::Initializer &meta
			);

			template<typename ClassType>
			void SetArrayConstructor(void);

			const RConstructor &GetConstructor(
				const RInvokableSignature &signature
			);

			const RConstructor &GetDynamicConstructor(
				const RInvokableSignature &signature
			);

			template<typename ClassType>
			void SetDestructor(void);

			// Method Getter, Method Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
			void AddField(
				const std::string &name,
				GetterReturnType(ClassType::*methodGetter)(void),
				void (ClassType::*methodSetter)(SetterArgumentType),
				const RMetaData::Initializer &meta
			);

			// Const Method Getter, Method Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
			void AddField(
				const std::string &name,
				GetterReturnType(ClassType::*methodGetter)(void) const,
				void (ClassType::*methodSetter)(SetterArgumentType),
				const RMetaData::Initializer &meta
			);

			// Method Getter, Field Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType>
			void AddField(
				const std::string &name,
				GetterReturnType(ClassType::*methodGetter)(void),
				typename RFieldSetter<ClassType, FieldType, false>::Signature fieldSetter,
				const RMetaData::Initializer &meta
			);

			// Const Method Getter, Field Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType>
			void AddField(
				const std::string &name,
				GetterReturnType(ClassType::*methodGetter)(void) const,
				typename RFieldSetter<ClassType, FieldType, false>::Signature fieldSetter,
				const RMetaData::Initializer &meta
			);

			// Field Getter, Method Setter
			template<typename ClassType, typename FieldType, typename SetterArgumentType>
			void AddField(
				const std::string &name,
				typename RFieldGetter<ClassType, FieldType, false>::Signature fieldGetter,
				void (ClassType::*methodSetter)(SetterArgumentType),
				const RMetaData::Initializer &meta
			);

			// Field Getter, Field Setter
			template<typename ClassType, typename FieldType>
			void AddField(
				const std::string &name,
				typename RFieldGetter<ClassType, FieldType, false>::Signature fieldGetter,
				typename RFieldSetter<ClassType, FieldType, false>::Signature fieldSetter,
				const RMetaData::Initializer &meta
			);

			const RField &GetField(const std::string &name) const;

			// Method Getter, Method Setter
			template<typename ClassType, typename FieldType, typename GetterType, typename SetterType>
			void AddStaticField(
				const std::string &name,
				GetterType getter,
				SetterType setter,
				const RMetaData::Initializer &meta
			);

			// Method Getter, Field Setter
			template<typename ClassType, typename FieldType, typename GetterType>
			void AddStaticField(
				const std::string &name,
				GetterType getter,
				FieldType *fieldSetter,
				const RMetaData::Initializer &meta
			);

			// Field Getter, Method Setter
			template<typename ClassType, typename FieldType, typename SetterType>
			void AddStaticField(
				const std::string &name,
				FieldType *fieldGetter,
				SetterType setter,
				const RMetaData::Initializer &meta
			);

			// Field Getter, Field Setter
			template<typename ClassType, typename FieldType>
			void AddStaticField(
				const std::string &name,
				FieldType *fieldGetter,
				FieldType *fieldSetter,
				const RMetaData::Initializer &meta
			);

			const RVariable &GetStaticField(const std::string &name) const;

			template<typename MethodType>
			void AddMethod(
				const std::string &name,
				MethodType method,
				const RMetaData::Initializer &meta
			);

			const RMethod &GetMethod(
				const std::string &name
			);

			const RMethod &GetMethod(
				const std::string &name,
				const RInvokableSignature &signature
			);

			template<typename ClassType, typename FunctionType>
			void AddStaticMethod(
				const std::string &name,
				FunctionType function,
				const RMetaData::Initializer &meta
			);

			const RFunction &GetStaticMethod(
				const std::string &name
			);

			const RFunction &GetStaticMethod(
				const std::string &name,
				const RInvokableSignature &signature
			);

			template<typename EnumType>
			void SetEnum(
				const std::string &name,
				const typename REnumContainer<EnumType>::Initializer &initalizer
			);

			bool			_bIsEnum;
			bool			_bIsPrimitive;
			bool			_bIsSigned;
			bool			_bIsFloatingPoint;
			bool			_bIsPointer;
			bool			_bIsClass;

			std::string		_name;
			REnum			_enum;
			std::set<RType> _baseClasses;
			std::set<RType> _derivedClasses;
			RMetaData		_meta;

			std::unordered_map<RInvokableSignature, RConstructor> _constructors;
			std::unordered_map<RInvokableSignature, RConstructor> _dynamicConstructors;

			RDestructor _destructor;
			RConstructor _arrayConstructor;

			std::vector<RField> _fields;
			std::vector<RVariable> _staticFields;

			std::unordered_map<std::string, InvokableOverloadMap<RMethod>> _methods;
			std::unordered_map<std::string, InvokableOverloadMap<RFunction>> _staticMethods;
		};
	}
}
