#pragma once

namespace Raven
{
	namespace Meta
	{
		// Getter for if this variable is a method.
		template<typename GlobalType>
		class RVariableGetter<GlobalType, true> : public RVariableGetterBase
		{
		public:
			typedef GlobalType(*RSignature)(void);

			RVariableGetter(RSignature function)
				: _function(function) { }

			RVariant GetValue(void) override
			{
				return _function();
			}

		private:
			RSignature _function;
		};

		// Getter for global variable.
		template<typename GlobalType>
		class RVariableGetter<GlobalType, false> : public RVariableGetterBase
		{
		public:
			RVariableGetter(GlobalType *global)
				: _global(global) { }

			RVariant GetValue(void) override
			{
				return* _variable;
			}

		private:
			GlobalType* _variable;
		};
	}
}