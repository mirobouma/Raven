#include "core/reflection/reflection_database.h"
#include "core/reflection/invokable/function.h"
#include <algorithm>
#include "core/reflection/type/type_registrant.h"

#define REGISTER_NATIVE_TYPE(type)                    \
    {                                                 \
        auto id = AllocateType( #type );              \
        auto handle = _types[ id ].get();                   \
                                                      \
        RTypeRegistrant<type>::Register( id, handle, true ); \
    }                                                 \

#define REGISTER_NATIVE_TYPE_VARIANTS(type) \
    REGISTER_NATIVE_TYPE( type )            \
    REGISTER_NATIVE_TYPE( type* )           \
    REGISTER_NATIVE_TYPE( const type* )     \

#define REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(type)         \
    REGISTER_NATIVE_TYPE_VARIANTS( type )                   \
    _types[ typeidof( type ) ].get()->SetArrayConstructor<type>( ); \

namespace Raven
{
	namespace Meta
	{
		RReflectionDatabase::RReflectionDatabase()
		{
			// Type 0 is reserved for an invalid type.
			AllocateType("INVALID_TYPE");
			
			// register all of the native type variants explicity, before
			// anything else
			REGISTER_NATIVE_TYPE_VARIANTS(void);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(int);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(unsigned int);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(bool);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(float);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(double);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(std::string);

			auto &stringType = _types[typeidof(std::string)];

			// explicitly add default constructors for string
			stringType.get()->AddConstructor<std::string, false, false>({ });
			stringType.get()->AddConstructor<std::string, false, true>({ });
		}

		RReflectionDatabase::~RReflectionDatabase()
		{
		}

		bool RReflectionDatabase::DeallocType(std::string typeName)
		{
			auto idItr = _ids.find(typeName);

			if (idItr != _ids.end())
			{
				FTypeID id = idItr->second;
				auto typeItr = _types.find(id);

				if (typeItr != _types.end())
				{
					_ids.erase(idItr);
					_types.erase(typeItr);

					return true;
				}
				else
				{
					//We found an ID entry but not an actual type. Unexpected behavior.
					return false;
				}
			
				return true;
			}

			return true;
		}

		bool RReflectionDatabase::DeallocType(FTypeID typeId)
		{
			auto idItr = std::find_if(std::begin(_ids), std::end(_ids),
				[=](auto&& p) { return p.second == typeId; });

			if (idItr != _ids.end())
			{
				auto typeItr = _types.find(typeId);

				if (typeItr != _types.end())
				{
					_ids.erase(idItr);
					_types.erase(typeItr);

					return true;
				}
				else
				{
					//We found an ID entry but not an actual type. Unexpected behavior.
					return false;
				}

				return true;
			}

			return true;
		}

		/*! Retrieve id of a specified type. */
		FTypeID RReflectionDatabase::GetIdOfType(std::string typeName)
		{
			auto it = _ids.find(typeName);
			if (it != _ids.end())
			{
				return it->second;
			}

			return InvalidTypeID;
		}

		FTypeID RReflectionDatabase::AllocateType(std::string typeName)
		{
			if (_ids.find(typeName) != _ids.end())
			{
				return InvalidTypeID;
			}

			// Retrieve new type ID.
			FTypeID id = _nextTypeID++;

			// Assign type ID to this type name.
			_ids[typeName] = id;

			_types[id] = std::unique_ptr<RTypeData>(new RTypeData(typeName));

			// return Type ID for this type.
			return id;
		}

		const RFunction &RReflectionDatabase::GetGlobalFunction(
			const std::string &name
		)
		{
			auto &base = _globalFunctions[name];

			if (!base.size())
				return RFunction::Invalid();

			return base.begin()->second;
		}

		const RFunction &RReflectionDatabase::GetGlobalFunction(
			const std::string &name,
			const RInvokableSignature &signature
		)
		{
			auto &base = _globalFunctions[name];

			auto search = base.find(signature);

			if (search == base.end())
				return RFunction::Invalid();

			return search->second;
		}
	}
}