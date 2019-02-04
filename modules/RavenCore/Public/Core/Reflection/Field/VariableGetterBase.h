#pragma once

namespace Raven
{
	namespace Meta
	{
		class RVariant;

		class RVariableGetterBase
		{
		public:
			virtual ~RVariableGetterBase() { }
			virtual RVariant GetValue() = 0;
		};
	}
}