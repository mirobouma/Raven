#pragma once

#include "ArrayWrapperContainer.h"

namespace Raven
{
	namespace Meta
	{
		template<typename T>
		RArrayWrapper::RArrayWrapper(TArray<T> &rhs)
			: _isConst(false)
			, _base(new RArrayWrapperContainer<T>(rhs))
		{

		}

		template<typename T>
		RArrayWrapper::RArrayWrapper(const TArray<T> &rhs)
			: _isConst(true)
			, _base(new RArrayWrapperContainer<T>(const_cast<TArray<T> &>(rhs)))
		{

		}
	}
}
