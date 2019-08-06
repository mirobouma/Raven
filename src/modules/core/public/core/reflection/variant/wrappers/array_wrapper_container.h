#pragma once
#include "array_wrapper_base.h"

namespace Raven
{
	namespace Meta
	{
		class RVariant;
		class RArgument;

		template<typename T>
		class RArrayWrapperContainer : public RArrayWrapperBase
		{
		public:
			RArrayWrapperContainer(TArray<T> &a);

			RVariant GetValue(size_t index) override;
			void SetValue(size_t index, const RArgument &value) override;

			void Insert(size_t index, const RArgument &value) override;
			void Remove(size_t index) override;

			size_t Size(void) const override;

		private:
			TArray<T> &_array;
		};
	}
}

#include "array_wrapper_container.hpp"