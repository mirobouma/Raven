#pragma once

namespace Raven
{
	namespace Meta
	{
		class RArgument;
		class RVariableSetterBase
		{
		public:
			virtual ~RVariableSetterBase() { }
			virtual void SetValue(const RArgument &value) = 0;
		};
	}
}