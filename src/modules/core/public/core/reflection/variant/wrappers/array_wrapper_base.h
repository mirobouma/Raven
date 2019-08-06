#pragma once
#include "core/containers/array.h"

namespace Raven
{
	namespace Meta
	{
		class RVariant;
		class RArgument;

		class RArrayWrapperBase
		{
		public:
			virtual ~RArrayWrapperBase(void) { }

			virtual RVariant GetValue(size_t index) = 0;
			virtual void SetValue(size_t index, const RArgument &value) = 0;

			virtual void Insert(size_t index, const RArgument &value) = 0;
			virtual void Remove(size_t index) = 0;

			virtual size_t Size(void) const = 0;
		};
	}
}