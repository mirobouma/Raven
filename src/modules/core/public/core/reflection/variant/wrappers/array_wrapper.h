#pragma once
#include "array_wrapper_base.h"

namespace Raven
{
	namespace Meta
	{
		class RArgument;
		class RArrayWrapper
		{
		public:
			RArrayWrapper(void);

			template<typename T>
			RArrayWrapper(TArray<T> &rhs);

			template<typename T>
			RArrayWrapper(const TArray<T> &rhs);

			RVariant GetValue(size_t index) const;
			void SetValue(size_t index, const RArgument &value);

			void Insert(size_t index, const RArgument &value);
			void Remove(size_t index);

			size_t Size(void) const;

			bool IsValid(void) const;
			bool IsConst(void) const;

		private:
			bool _isConst;
			RArrayWrapperBase* _base;
		};
	}
}

#include "array_wrapper.hpp"