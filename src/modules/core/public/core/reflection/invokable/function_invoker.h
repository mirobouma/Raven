#pragma once
#include "function_invoker_base.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ReturnType, typename ...ArgTypes>
		class RFunctionInvoker : public RFunctionInvokerBase
		{
		public:
			typedef ReturnType(*Signature)(ArgTypes...);

			static_assert(THIS_ARG_COUNT <= MaxArgumentCount,
				"Function has too many arguments. It's time to generate more overloads."
				);

			RFunctionInvoker(Signature function);

			RVariant Invoke(const ArgumentList &arguments) override;

		private:
			template<typename _>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
			RVariant invoke(const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
			RVariant invoke(const ArgumentList &arguments);

			Signature _function;
		};
	}
}

#include "function_invoker.hpp"
#include "void_function_invoker.hpp"