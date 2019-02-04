#pragma once
#include "Core/Reflection/Variant/VariantBase.h"

namespace Raven
{
	class RObject;
	namespace Meta
	{
		class RObjectWrapper : public RVariantBase
		{
		public:
			RObjectWrapper(RObject* instance);

			RType GetType(void) const override;
			void *GetPtr(void) const override;

			int ToInt(void) const override;
			bool ToBool(void) const override;
			float ToFloat(void) const override;
			double ToDouble(void) const override;
			std::string ToString(void) const override;

			RVariantBase* Clone(void) const override;
		private:
			RObject *_object;
		};
	}
}