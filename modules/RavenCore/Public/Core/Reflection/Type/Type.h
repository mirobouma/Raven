#pragma once

#include "TypeID.h"
#include "TypeConfig.h"
#include "Core/Reflection/Invokable/InvokableConfig.h"
#include "Core/Reflection/Invokable/ArgumentConfig.h"

#include <string>
#include <set>
#include <vector>
#include <functional>

namespace Raven
{
	namespace Meta
	{

		class RVariant;
		class REnum;
		class RConstructor;
		class RDestructor;
		class RField;
		class RVariable;
		class RMethod;
		class RFunction;
		class RArgument;

		class RMetaData;

		/*! Class describing an type. */
		class RType
		{
		public:
			template<typename T>
			friend class RTypeRegistrant;
			friend class RReflectionDatabase;
			friend class RTypeData;

			RType();
			RType(const RType &rhs);
			RType(FTypeID type, bool isArray = false);
			RType& operator=(const RType &rhs);

			operator bool(void) const;

			bool operator<(const RType &rhs) const;
			bool operator>(const RType &rhs) const;
			bool operator<=(const RType &rhs) const;
			bool operator>=(const RType &rhs) const;
			bool operator==(const RType &rhs) const;
			bool operator!=(const RType &rhs) const;

			/*! Get the invalid type. */
			static const RType &Invalid(void);

			/*! Gets a list of references to all the types registered in the reflection database. */
			static std::vector<RType> GetTypes(void);

			/*! Get a type by name. */
			static RType GetByName(const std::string &name);

			/*! Get the representation of a type by type. */
			template<typename T>
			static RType Get(T &&obj);

			/*! Get the ID of this type. */
			FTypeID GetID(void) const;

			static std::vector<RVariable> GetVariables(void);

			static const RVariable &GetVariable(const std::string &name);

			static std::vector<RFunction> GetGlobalFunctions(void);

			static const RFunction &GetGlobalFunction(const std::string &name);

			static const RFunction &GetGlobalFunction(
				const std::string &name,
				const RInvokableSignature &signature
			);

			bool IsValid(void) const;
			bool IsPrimitive(void) const;
			bool IsFloatingPoint(void) const;
			bool IsSigned(void) const;
			bool IsEnum(void) const;
			bool IsPointer(void) const;
			bool IsClass(void) const;
			bool IsArray(void) const;

			std::string GetName(void) const;
			const RMetaData &GetMeta(void) const;

			void Destroy(RVariant &instance) const;
			RType GetDecayedType(void) const;
			RType GetArrayType(void) const;

			const REnum &GetEnum(void) const;

			bool DerivesFrom(const RType &other) const;

			template<typename T>
			bool DerivesFrom() const;

			const std::set<RType> &GetBaseClasses(void) const;
			const std::set<RType> &GetDerivedClasses(void) const;

			std::vector<RConstructor> GetConstructors(void) const;
			std::vector<RConstructor> GetDynamicConstructors(void) const;

			const RConstructor &GetConstructor(
				const RInvokableSignature &signature = RInvokableSignature()
			) const;

	
			const RConstructor &GetDynamicConstructor(
				const RInvokableSignature &signature = RInvokableSignature()
			) const;

			const RConstructor &GetArrayConstructor(void) const;
			const RDestructor &GetDestructor(void) const;

			std::vector<RMethod> GetMethods(void) const;
			const RMethod &GetMethod(const std::string &name) const;

			const RMethod &GetMethod(
				const std::string &name,
				const RInvokableSignature &signature
			) const;

			std::vector<RFunction> GetStaticMethods(void) const;

			const RFunction &GetStaticMethod(const std::string &name) const;
			const RFunction &GetStaticMethod(
				const std::string &name,
				const RInvokableSignature &signature
			) const;

			const std::vector<RField> &GetFields(void) const;
			const RField &GetField(const std::string &name) const;

			std::vector<RVariable> GetStaticFields(void) const;
			const RVariable &GetStaticField(const std::string &name) const;

		protected:
			bool		_bIsArrayType; /*!< Flag describing if this type is an array type. */
			FTypeID		_typeID; /*!< Id of the type to retrieve type data information. */
			RTypeData*	_typeData; /*!< Pointer to type data to retrieve type data faster. */
		};
	}
}

#include "Type.hpp"