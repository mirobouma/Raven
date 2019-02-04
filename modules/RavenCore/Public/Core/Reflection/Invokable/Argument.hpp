#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename T>
		RArgument::RArgument(const T &data)
			: _typeID(typeidof(T))
			, _isArray(Meta::IsArray<T>::value)
			, _data(reinterpret_cast<const void*>(std::addressof(data)))
		{
			static_assert(!std::is_same< RArgument, T >::value,
				"Cannot use RArgument as an argument"
				);
		}

		template<typename T>
		RArgument::RArgument(T &data)
			: _typeID(typeidof(T))
			, _isArray(Meta::IsArray<T>::value)
			, _data(reinterpret_cast<const void*>(std::addressof(data)))
		{
			static_assert(!std::is_same< RArgument, T >::value,
				"Cannot use RArgument as an argument"
				);
		}

		template<typename T>
		T &RArgument::GetValue(void) const
		{
			return *reinterpret_cast<
				typename std::remove_reference< T >::type*
			>(
				const_cast<void *>(_data)
				);
		}
	}
}
