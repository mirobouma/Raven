#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		RMethodInvoker<ClassType, ReturnType, ArgTypes...>::RMethodInvoker(Signature method)
			: _method(reinterpret_cast<ConstSignature>(method)) { }

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		RMethodInvoker<ClassType, ReturnType, ArgTypes...>::RMethodInvoker(ConstSignature method)
			: _method(method) { }

		///////////////////////////////////////////////////////////////////////

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::Invoke(RVariant &obj, const ArgumentList &arguments)
		{
			RASSERT(arguments.size() == THIS_ARG_COUNT,
				"Invalid method arguments.\nExpected %i args but got %i.",
				THIS_ARG_COUNT,
				arguments.size()
			);

			return invoke<void, ArgTypes...>(obj, arguments);
		}

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)();
		}


		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
				arguments[0].GetValue<A1>()
				);
		}

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>()
				);
		}

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>()
				);
		}

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>()
				);
		}


		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>()
				);
		}


		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>(),
				arguments[5].GetValue<A6>()
				);
		}

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
				arguments[0].GetValue<A1>(),
				arguments[1].GetValue<A2>(),
				arguments[2].GetValue<A3>(),
				arguments[3].GetValue<A4>(),
				arguments[4].GetValue<A5>(),
				arguments[5].GetValue<A6>(),
				arguments[6].GetValue<A7>()
				);
		}

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
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

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
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

		template<typename ClassType, typename ReturnType, typename ... ArgTypes>
		template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
		RVariant RMethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(RVariant &obj, const ArgumentList &arguments)
		{
			auto &instance = obj.GetValue<ClassType>();

			return (instance.*_method)(
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
