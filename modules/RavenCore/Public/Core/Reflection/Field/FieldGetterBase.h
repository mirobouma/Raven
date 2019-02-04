#pragma once

namespace Raven
{
	namespace Meta
	{
		class RVariant;

		class RFieldGetterBase
		{
		public:
			virtual ~RFieldGetterBase(void) { }

			virtual RVariant GetValue(const RVariant &obj) = 0;
			virtual RVariant GetValueReference(const RVariant &obj) = 0;
		};
	}
}