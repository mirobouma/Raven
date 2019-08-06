#pragma once
#include "wrappers/array_wrapper.h"

namespace Raven
{
	namespace Meta
	{
		template<typename T, typename StorageType>
		RVariantContainerArray<T, StorageType>::RVariantContainerArray(StorageType &rhs)
			: _array(rhs)
		{

		}

		template<typename T, typename StorageType>
		RType RVariantContainerArray<T, StorageType>::GetType(void) const
		{
			return typeof(TArray<T>);
		}

		template<typename T, typename StorageType>
		void *RVariantContainerArray<T, StorageType>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(_array)
					)
				);
		}

		template<typename T, typename StorageType>
		int RVariantContainerArray<T, StorageType>::ToInt(void) const
		{
			return int();
		}

		template<typename T, typename StorageType>
		bool RVariantContainerArray<T, StorageType>::ToBool(void) const
		{
			return bool();
		}

		template<typename T, typename StorageType>
		float RVariantContainerArray<T, StorageType>::ToFloat(void) const
		{
			return float();
		}

		template<typename T, typename StorageType>
		double RVariantContainerArray<T, StorageType>::ToDouble(void) const
		{
			return double();
		}

		template<typename T, typename StorageType>
		std::string RVariantContainerArray<T, StorageType>::ToString(void) const
		{
			return std::string();
		}

		template<typename T, typename StorageType>
		bool RVariantContainerArray<T, StorageType>::IsArray(void) const
		{
			return true;
		}

		template<typename T, typename StorageType>
		RArrayWrapper RVariantContainerArray<T, StorageType>::GetArray(void) const
		{
			return RArrayWrapper(const_cast<TArray<T>&>(_array));
		}

		template<typename T, typename StorageType>
		RVariantBase *RVariantContainerArray<T, StorageType>::Clone(void) const
		{
			return new RVariantContainerArray<T, StorageType>(const_cast<TArray<T>&>(_array));
		}
	}
}
