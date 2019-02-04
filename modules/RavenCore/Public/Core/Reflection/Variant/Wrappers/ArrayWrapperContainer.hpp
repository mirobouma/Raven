#pragma once

#include "Core/Reflection/Variant/Variant.h"
#include "Core/Containers/Array.h"

namespace Raven
{
	namespace Meta
	{
		template<typename T>
		RArrayWrapperContainer<T>::RArrayWrapperContainer(TArray<T> &a)
			: _array(a)
		{

		}

		template<typename T>
		RVariant RArrayWrapperContainer<T>::GetValue(size_t index)
		{
			return { _array[index] };
		}

		template<typename T>
		void RArrayWrapperContainer<T>::SetValue(size_t index, const RArgument &value)
		{
			_array.at(index) = value.GetValue<T>();
		}

		template<typename T>
		void RArrayWrapperContainer<T>::Insert(size_t index, const RArgument &value)
		{
			_array.insert(_array.begin() + index, value.GetValue<T>());
		}

		template<typename T>
		void RArrayWrapperContainer<T>::Remove(size_t index)
		{
			_array.erase(_array.begin() + index);
		}

		template<typename T>
		size_t RArrayWrapperContainer<T>::Size(void) const
		{
			return _array.size();
		}
	}
}