#pragma once
#include <cinttypes>

namespace Raven
{
	namespace Meta
	{
		// TypeID is a typedef for an unsigned integer describing types.
		typedef uint32_t FTypeID;
		
		// We reserve ID 0 to be an invalid type.
		const FTypeID InvalidTypeID = 0;

		// Structure holding the typeID of a specific type, because of the template specializations this is unique per type.
		template<typename T>
		struct TypeIdOf
		{
			static FTypeID ID;
		};

		template<typename T>
		FTypeID TypeIdOf<T>::ID = InvalidTypeID;
	}
}