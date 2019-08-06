#pragma once
#include "core/containers/array.h"

namespace Raven
{
	namespace Meta
	{
		template<typename T, typename = void>
		struct TypeOrEnumType
		{
			typedef T type;
		};

		template<typename T>
		struct TypeOrEnumType<T,
			typename std::enable_if<
			std::is_enum<T>::value
			>::type
		>
		{
			typedef typename std::underlying_type<T>::type type;
		};

		template<typename T>
		struct RemoveArray
		{
			typedef T type;
		};

		template<typename T>
		struct RemoveArray<TArray<T>>
		{
			typedef T type;
		};

		///////////////////////////////////////////////////////////////////////////

		template<typename T>
		struct IsArray
		{
			static const bool value = false;
		};

		template<typename T>
		struct IsArray<TArray<T>>
		{
			static const bool value = true;
		};

		///////////////////////////////////////////////////////////////////////////

		template<typename T>
		using ArrayByValue = TArray<T>;

		template<typename T>
		using ArrayByReference = typename std::add_lvalue_reference<TArray<T>>::type;
	}
}
