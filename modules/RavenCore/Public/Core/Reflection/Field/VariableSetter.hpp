#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename GlobalType>
		class RVariableSetter<GlobalType, true> : public RVariableSetterBase
		{
		public:
			typedef GlobalType(*RSignature)(const GlobalType &);

			RVariableSetter(RSignature function)
				: _function(function) { }

			void SetValue(const RArgument &value) override
			{
				_function(value.GetValue<GlobalType>());
			}

		private:
			RSignature _function;
		};

		template<typename GlobalType>
		class RVariableSetter<GlobalType, false> : public RVariableSetterBase
		{
		public:
			RVariableSetter(GlobalType *field)
				: _field(field) { }

			void SetValue(const RArgument &value) override
			{
				const_cast<GlobalType&>(*_field) = value.GetValue<GlobalType>();
			}

		private:
			GlobalType *_field;
		};
	}
}