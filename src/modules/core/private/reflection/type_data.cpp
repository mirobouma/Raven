#include "core/reflection/type/type_data.h"
#include "core/reflection/reflection_database.h"
#include "core/reflection/invokable/method.h"
#include "core/reflection/invokable/constructor.h"
#include "core/reflection/invokable/destructor.h"
#include "core/reflection/invokable/function.h"

namespace Raven
{
	namespace Meta
	{
		RTypeData::RTypeData(void)
			: _bIsEnum(false)
			, _bIsPrimitive(false)
			, _bIsPointer(false)
			, _bIsClass(false)
			, _enum { nullptr }  { }

		

		RTypeData::RTypeData(const std::string &name)
			: _bIsEnum(false)
			, _bIsPrimitive(false)
			, _bIsPointer(false)
			, _bIsClass(false)
			, _name(name)
			, _enum{ nullptr } { }

		void RTypeData::LoadBaseClasses(
			RReflectionDatabase &db,
			FTypeID thisType,
			const std::initializer_list<RType> &classes
		)
		{
			for (auto &base : classes)
			{
				// skip invalid types
				if (base == RType::Invalid())
					continue;

				_baseClasses.insert(base);

				// add this type to the base type's derived classes
				db._types[base._typeID].get()->_derivedClasses.insert(thisType);
			}
		}

		

		const RConstructor &RTypeData::GetConstructor(
			const RInvokableSignature &signature
		)
		{
			auto search = _constructors.find(signature);

			if (search == _constructors.end())
				return RConstructor::Invalid();

			return search->second;
		}

		const RConstructor &RTypeData::GetDynamicConstructor(
			const RInvokableSignature &signature
		)
		{
			auto search = _dynamicConstructors.find(signature);

			if (search == _dynamicConstructors.end())
				return RConstructor::Invalid();

			return search->second;
		}


		const RField &RTypeData::GetField(const std::string &name) const
		{
			for (auto &field : _fields)
				if (field.GetName() == name)
					return field;

			return RField::Invalid();
		}

		const RVariable &RTypeData::GetStaticField(const std::string &name) const
		{
			for (auto &field : _staticFields)
				if (field.GetName() == name)
					return field;

			return RVariable::Invalid();
		}

		const RMethod &RTypeData::GetMethod(const std::string &name)
		{
			auto &base = _methods[name];

			if (!base.size())
				return RMethod::Invalid();

			return base.begin()->second;
		}

		const RMethod &RTypeData::GetMethod(
			const std::string &name,
			const RInvokableSignature &signature
		)
		{
			auto &base = _methods[name];

			auto search = base.find(signature);

			if (search == base.end())
				return RMethod::Invalid();

			return search->second;
		}

		const RFunction &RTypeData::GetStaticMethod(const std::string &name)
		{
			auto &base = _staticMethods[name];

			if (!base.size())
				return RFunction::Invalid();

			return base.begin()->second;
		}

		const RFunction &RTypeData::GetStaticMethod(
			const std::string &name,
			const RInvokableSignature &signature
		)
		{
			auto &base = _staticMethods[name];

			auto search = base.find(signature);

			if (search == base.end())
				return RFunction::Invalid();

			return search->second;
		}
	}
}
