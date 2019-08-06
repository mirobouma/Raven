#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename ReturnType, typename ... ArgTypes>
		RFunctionInvoker<ReturnType, ArgTypes...>::RFunctionInvoker(Signature function)
			: _function(function) { }

		template<typename ReturnType, typename ... ArgTypes>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::Invoke(const ArgumentList &arguments)
		{
			RASSERT_MSG(arguments.size() == THIS_ARG_COUNT,
				"Invalid function arguments.\nExpected %i args but got %i.",
				THIS_ARG_COUNT,
				arguments.size()
			);

			return invoke<void, ArgTypes...>(arguments);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function();
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>(),
				arguments[5].GetValue<A6>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>(),
				arguments[5].GetValue<A6>(),
				arguments[6].GetValue<A7>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>(),
				arguments[5].GetValue<A6>(),
				arguments[6].GetValue<A7>(),
				arguments[7].GetValue<A8>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>(),
				arguments[5].GetValue<A6>(),
				arguments[6].GetValue<A7>(),
				arguments[7].GetValue<A8>(),
				arguments[8].GetValue<A9>()
			);
		}

		template<typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
		RVariant RFunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
		{
			return _function(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>(),
				arguments[5].GetValue<A6>(),
				arguments[6].GetValue<A7>(),
				arguments[7].GetValue<A8>(),
				arguments[8].GetValue<A9>(),
				arguments[9].GetValue<A10>()
			);
		}
	}
}
