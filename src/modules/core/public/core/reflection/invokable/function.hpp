#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename ReturnType, typename ...ArgTypes>
		RFunction::RFunction(
			const std::string &name,
			ReturnType(*function)(ArgTypes...),
			RType parentType
		)
			: RInvokable(name)
			, _parentType(parentType)
			, _invoker(new RFunctionInvoker<ReturnType, ArgTypes...>(function))
		{
			FTypeUnpacker<ArgTypes...>::Apply(_signature);
		}

		template<typename ...Args>
		RVariant RFunction::Invoke(Args &&...args) const
		{
			ArgumentList arguments{ std::forward<Args>(args)... };

			return InvokeVariadic(arguments);
		}
	}
}
