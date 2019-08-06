#include "core/reflection/type/type.h"
#include "core/reflection/reflection_database.h"
#include "core/debug/log_manager.h"

#define gDatabase RReflectionDatabase::Get()

namespace Raven
{
	namespace Meta
	{
		RType::RType(void)
			: _typeID(InvalidTypeID)
			, _bIsArrayType(false) { }

		RType::RType(const RType &rhs)
			: _typeID(rhs._typeID)
			, _bIsArrayType(rhs._bIsArrayType) { }

		RType::RType(FTypeID id, bool isArray)
			: _typeID(id)
			, _bIsArrayType(isArray) { }

		RType::operator bool(void) const
		{
			return _typeID != InvalidTypeID;
		}

		RType &RType::operator=(const RType &rhs)
		{
			_typeID = rhs._typeID;
			_bIsArrayType = rhs._bIsArrayType;

			return *this;
		}

		bool RType::operator<(const RType &rhs) const
		{
			return _typeID < rhs._typeID;
		}

		bool RType::operator>(const RType &rhs) const
		{
			return _typeID > rhs._typeID;
		}

		bool RType::operator<=(const RType &rhs) const
		{
			return _typeID <= rhs._typeID;
		}

		bool RType::operator>=(const RType &rhs) const
		{
			return _typeID >= rhs._typeID;
		}

		bool RType::operator==(const RType &rhs) const
		{
			return _typeID == rhs._typeID;
		}

		bool RType::operator!=(const RType &rhs) const
		{
			return _typeID != rhs._typeID;
		}

		const RType &RType::Invalid(void)
		{
			static const RType invalid{ InvalidTypeID };
			return invalid;
		}

		FTypeID RType::GetID(void) const
		{
			return _typeID;
		}

		std::vector<RType> RType::GetTypes(void)
		{
			auto count = gDatabase._types.size();

			std::vector<RType> types;

			// skip the first one, as it's reserved for unknown
			for (FTypeID i = 1; i < count; ++i)
				types.emplace_back(i);

			return types;
		}

		

		std::vector<RVariable> RType::GetVariables(void)
		{
			std::vector<RVariable> globals;

			for (auto &global : gDatabase._globals)
				globals.emplace_back(global.second);

			return globals;
		}

		

		const RVariable &RType::GetVariable(const std::string &name)
		{
			return gDatabase._globals[name];
		}

		

		std::vector<RFunction> RType::GetGlobalFunctions(void)
		{
			std::vector<RFunction> functions;

			for (auto &overload : gDatabase._globalFunctions)
			{
				for (auto &function : overload.second)
				{
					functions.emplace_back(function.second);
				}
			}

			return functions;
		}

		

		const RFunction& RType::GetGlobalFunction(const std::string &name)
		{
			return gDatabase.GetGlobalFunction(name);
		}

		

		const RFunction& RType::GetGlobalFunction(
			const std::string &name,
			const RInvokableSignature &signature
		)
		{
			return gDatabase.GetGlobalFunction(name, signature);
		}

		

		RType RType::GetByName(const std::string& name)
		{
			auto search = gDatabase._ids.find(name);

			if (search == gDatabase._ids.end())
				return Invalid();

			return { search->second };
		}

		bool RType::IsValid(void) const
		{
			return _typeID != InvalidTypeID;
		}

		

		bool RType::IsPrimitive(void) const
		{
			return gDatabase._types[_typeID].get()->_bIsPrimitive;
		}

		

		bool RType::IsFloatingPoint(void) const
		{
			return gDatabase._types[_typeID].get()->_bIsFloatingPoint;
		}

		

		bool RType::IsSigned(void) const
		{
			return gDatabase._types[_typeID].get()->_bIsSigned;
		}

		

		bool RType::IsEnum(void) const
		{
			return gDatabase._types[_typeID].get()->_bIsEnum;
		}

		

		bool RType::IsPointer(void) const
		{
			return gDatabase._types[_typeID].get()->_bIsPointer;
		}

		

		bool RType::IsClass(void) const
		{
			return gDatabase._types[_typeID].get()->_bIsClass;
		}

		

		bool RType::IsArray(void) const
		{
			return _bIsArrayType;
		}

		

		std::string RType::GetName(void) const
		{
			auto &name = gDatabase._types[_typeID].get()->_name;

			if (IsArray())
				return "Array<" + name + ">";

			return name;
		}

		const RMetaData& RType::GetMeta(void) const
		{
			return gDatabase._types[_typeID].get()->_meta;
		}

		

		void RType::Destroy(RVariant &instance) const
		{
			auto &destructor = gDatabase._types[_typeID].get()->_destructor;

			if (destructor.IsValid())
				destructor.Invoke(instance);
		}

		RType RType::GetDecayedType(void) const
		{
			RASSERT(false, "Type::GetDecayedType() not implemented.");

			return RType();
		}

		
		RType RType::GetArrayType(void) const
		{
			return RType(_typeID, false);
		}	

		const REnum &RType::GetEnum(void) const
		{
			return gDatabase._types[_typeID].get()->_enum;
		}


		bool RType::DerivesFrom(const RType &other) const
		{
			auto &baseClasses = gDatabase._types[_typeID].get()->_baseClasses;

			return baseClasses.find(other) != baseClasses.end();
		}

		

		const std::set<RType> &RType::GetBaseClasses(void) const
		{
			return gDatabase._types[_typeID].get()->_baseClasses;
		}

		

		const std::set<RType> &RType::GetDerivedClasses(void) const
		{
			return gDatabase._types[_typeID].get()->_derivedClasses;
		}

		

		std::vector<RConstructor> RType::GetConstructors(void) const
		{
			auto &handle = gDatabase._types[_typeID].get()->_constructors;

			std::vector<RConstructor> constructors;

			for (auto &constructor : handle)
				constructors.emplace_back(constructor.second);

			return constructors;
		}

		

		std::vector<RConstructor> RType::GetDynamicConstructors(void) const
		{
			auto &handle = gDatabase._types[_typeID].get()->_dynamicConstructors;

			std::vector<RConstructor> constructors;

			for (auto &constructor : handle)
				constructors.emplace_back(constructor.second);

			return constructors;
		}

		

		const RConstructor &RType::GetConstructor(
			const RInvokableSignature &signature
		) const
		{
			return gDatabase._types[_typeID].get()->GetConstructor(signature);
		}

		

		const RConstructor &RType::GetDynamicConstructor(
			const RInvokableSignature &signature
		) const
		{
			return gDatabase._types[_typeID].get()->GetDynamicConstructor(signature);
		}

		

		const RConstructor &RType::GetArrayConstructor(void) const
		{
			return gDatabase._types[_typeID].get()->_arrayConstructor;
		}

		

		const RDestructor &RType::GetDestructor(void) const
		{
			return gDatabase._types[_typeID].get()->_destructor;
		}

		

		std::vector<RMethod> RType::GetMethods(void) const
		{
			std::vector<RMethod> methods;

			auto &handle = gDatabase._types[_typeID].get()->_methods;

			for (auto &overload : handle)
			{
				for (auto &method : overload.second)
				{
					methods.emplace_back(method.second);
				}
			}

			return methods;
		}

		

		const RMethod &RType::GetMethod(const std::string &name) const
		{
			return gDatabase._types[_typeID].get()->GetMethod(name);
		}

		

		const RMethod &RType::GetMethod(
			const std::string &name,
			const RInvokableSignature &signature
		) const
		{
			return gDatabase._types[_typeID].get()->GetMethod(name, signature);
		}

		

		std::vector<RFunction> RType::GetStaticMethods(void) const
		{
			std::vector<RFunction> methods;

			auto &handle = gDatabase._types[_typeID].get()->_staticMethods;

			for (auto &overload : handle)
			{
				for (auto &method : overload.second)
				{
					methods.emplace_back(method.second);
				}
			}

			return methods;
		}

		

		const RFunction &RType::GetStaticMethod(const std::string &name) const
		{
			return gDatabase._types[_typeID].get()->GetStaticMethod(name);
		}

		

		const RFunction &RType::GetStaticMethod(
			const std::string &name,
			const RInvokableSignature &signature
		) const
		{
			return gDatabase._types[_typeID].get()->GetStaticMethod(name, signature);
		}

		

		const std::vector<RField> &RType::GetFields(void) const
		{
			return gDatabase._types[_typeID].get()->_fields;
		}

		

		const RField &RType::GetField(const std::string &name) const
		{
			return gDatabase._types[_typeID].get()->GetField(name);
		}

		

		std::vector<RVariable> RType::GetStaticFields(void) const
		{
			return gDatabase._types[_typeID].get()->_staticFields;
		}

		

		const RVariable &RType::GetStaticField(const std::string &name) const
		{
			return gDatabase._types[_typeID].get()->GetStaticField(name);
		}
	}
}