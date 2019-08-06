#pragma once

#include "type_unpacker.hpp"

namespace Raven
{
	namespace Meta
	{
		template<class ClassType, typename ReturnType, typename ...ArgTypes>
		RMethod::RMethod(
			const std::string &name,
			ReturnType(ClassType::*RMethod)(ArgTypes...)
		)
			: RInvokable(name)
			, _isConst(false)
			, _classType(typeof(ClassType))
			, _invoker(new RMethodInvoker<ClassType, ReturnType, ArgTypes...>(RMethod))
		{
			FTypeUnpacker<ArgTypes...>::Apply(m_signature);
		}

		template<class ClassType, typename ReturnType, typename ...ArgTypes>
		RMethod::RMethod(
			const std::string &name,
			ReturnType(ClassType::*RMethod)(ArgTypes...) const
		)
			: RInvokable(name)
			, _isConst(true)
			, _classType(typeof(ClassType))
			, _invoker(new RMethodInvoker<ClassType, ReturnType, ArgTypes...>(RMethod))
		{
			FTypeUnpacker<ArgTypes...>::Apply(m_signature);
		}

		///////////////////////////////////////////////////////////////////////

		template<typename ...Args>
		RVariant RMethod::Invoke(RVariant &instance, Args &&...args) const
		{
			ArgumentList arguments{ std::forward<Args>(args)... };

			return Invoke(instance, arguments);
		}
	}
}