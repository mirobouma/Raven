#pragma once

#include "Wrappers/ObjectWrapper.h"
#include "VariantContainerArray.h"
#include "Core/Reflection/Macro.h"

namespace Raven
{
	namespace Meta
	{
		template <typename T>
		RVariant::RVariant(
			T *data,
			Variant_Policy::WrapObject,
			typename std::enable_if<
			std::is_base_of<RObject, T>::value
			>::type*
		)
			: _isConst(std::is_const<T>::value)
			, _data(
				new RObjectWrapper(
					static_cast<RObject*>(
						const_cast<typename std::remove_const<T>::type*>(
							data
							)
						)
				)
			)
		{

		}

		template<typename T>
		RVariant::RVariant(
			T &data,
			DISABLE_VARIANT
		)
			: _isConst(std::is_pointer<T>::value && std::is_const<T>::value)
			, _data(new RVariantContainer<CleanedType<T> >(data))
		{

		}

		template<typename T>
		RVariant::RVariant(
			T &data,
			Variant_Policy::NoCopy,
			DISABLE_VARIANT
		)
			: _isConst(std::is_pointer<T>::value && std::is_const<T>::value)
			, _data(new RVariantContainer< CleanedType<T>& >(data))
		{

		}

		template<typename T>
		RVariant::RVariant(
			T &&data,
			DISABLE_VARIANT,
			DISABLE_ARGUMENT,
			DISABLE_CONST
		)
			: _isConst(false)
			, _data(
				new RVariantContainer< CleanedType<T> >(
					static_cast<T&&>(data)
					)
			)
		{

		}

		template<typename T>
		RVariant::RVariant(TArray<T> &rhs)
			: _isConst(false)
			, _data(new RVariantContainerArray<T, Meta::ArrayByReference<T>>(rhs))
		{

		}

		template<typename T>
		RVariant::RVariant(const TArray<T> &rhs)
			: _isConst(true)
			, _data(new RVariantContainerArray<T, const Meta::ArrayByReference<T>>(const_cast<const Meta::ArrayByReference<T>>(rhs)))
		{

		}

		template<typename T>
		RVariant::RVariant(TArray<T> &&rhs)
			: _isConst(false)
			, _data(new RVariantContainerArray<T, Meta::ArrayByValue<T>>(rhs))
		{

		}

		template<typename T>
		RVariant::RVariant(const TArray<T> &&rhs)
			: _isConst(true)
			, _data(new RVariantContainerArray<T, const Meta::ArrayByValue<T>>(rhs))
		{

		}

		template<typename T>
		RVariant &RVariant::operator=(T &&rhs)
		{
			RVariant(static_cast<T&&>(rhs)).Swap(*this);

			return *this;
		}

		template<typename T>
		T &RVariant::GetValue(void) const
		{
			return *static_cast<T*>(getPtr());
		}
	}
}

