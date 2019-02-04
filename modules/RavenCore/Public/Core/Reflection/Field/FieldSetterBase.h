#pragma once

namespace Raven
{
	namespace Meta
	{
		class RVariant;
		class RFieldSetterBase
		{
		public:
			virtual ~RFieldSetterBase(void) { }
			virtual void SetValue(RVariant &obj, const RVariant &value) = 0;
		};
	}
}