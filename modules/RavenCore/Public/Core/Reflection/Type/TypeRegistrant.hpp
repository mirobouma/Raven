#pragma once

#include "Type.h"
#include "TypeID.h"

namespace Raven
{
	namespace Meta
	{
		template<typename T>
		bool RTypeRegistrant<T>::bDefined = false;

		template<typename T>
		void RTypeRegistrant<T>::Register(
			FTypeID id,
			RTypeData* data,
			bool beingDefined)
		{
			// already defined.
			if (id == InvalidTypeID)
				return;

			// Assign type id so we can use our typeof macro's.
			TypeIdOf<T>::ID = id;

			typedef typename std::remove_pointer< typename std::decay<T>::type >::type Decayed;

			data->_bIsClass = std::is_class< Decayed >::value;
			data->_bIsEnum = std::is_enum< Decayed >::value;
			data->_bIsPointer = std::is_pointer< T >::value;
			data->_bIsPrimitive = std::is_arithmetic< Decayed >::value;
			data->_bIsFloatingPoint = std::is_floating_point< Decayed >::value;
			data->_bIsSigned = std::is_signed< Decayed >::value;

			if (beingDefined)
			{
				addDefaultConstructor(*data);
				applyTrivialAttributes(*data);
			}
		}

		template<typename T>
		template<typename U>
		void RTypeRegistrant<T>::addDefaultConstructor(
			RTypeData &data,
			typename std::enable_if<
			!IsTriviallyDefaultConstructible(U)
			>::type*
		)
		{
			// do nothing
		}

		template<typename T>
		template<typename U>
		void RTypeRegistrant<T>::addDefaultConstructor(
			RTypeData &data,
			typename std::enable_if<
			IsTriviallyDefaultConstructible(U)
			>::type*
		)
		{
			// add the good 'ol default constructor
			data.AddConstructor<T, false, false>({ });

			// add the good 'ol dynamic default constructor
			data.AddConstructor<T, true, false>({ });
		}

		template<typename T>
		template<typename U>
		void RTypeRegistrant<T>::applyTrivialAttributes(
			RTypeData &data,
			typename std::enable_if<
			!std::is_trivial<U>::value
			>::type*
		)
		{
			// do nothing
		}

		template<typename T>
		template<typename U>
		void RTypeRegistrant<T>::applyTrivialAttributes(
			RTypeData &data,
			typename std::enable_if<
			std::is_trivial<U>::value
			>::type*
		)
		{
			data.SetDestructor<T>();
		}
	}
}

