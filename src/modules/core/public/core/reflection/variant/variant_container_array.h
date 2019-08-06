#pragma once
#include "variant_base.h"

namespace Raven
{
	namespace Meta
	{
		class RArrayWrapper;

		template<typename T, typename StorageType>
		class RVariantContainerArray : public RVariantBase
		{
		public:
			RVariantContainerArray(StorageType &rhs);

			RType GetType(void) const override;
			void *GetPtr(void) const override;

			int ToInt(void) const override;
			bool ToBool(void) const override;
			float ToFloat(void) const override;
			double ToDouble(void) const override;
			std::string ToString(void) const override;

			bool IsArray(void) const override;
			RArrayWrapper GetArray(void) const override;

			RVariantBase *Clone(void) const override;

		private:
			StorageType _array;
		};
	}
}

#include "variant_container_array.hpp"