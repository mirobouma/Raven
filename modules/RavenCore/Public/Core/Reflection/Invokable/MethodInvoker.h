#pragma once

#include "MethodInvokerBase.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType, typename ReturnType, typename ...ArgTypes>
		class RMethodInvoker : public RMethodInvokerBase
		{
		public:
			typedef ReturnType(ClassType::*Signature)(ArgTypes...);
			typedef ReturnType(ClassType::*ConstSignature)(ArgTypes...) const;

			static_assert(THIS_ARG_COUNT <= MaxArgumentCount,
				"Method has too many arguments. It's time to generate more overloads."
				);

			RMethodInvoker(Signature method);
			RMethodInvoker(ConstSignature method);

			RVariant Invoke(RVariant &obj, const ArgumentList &arguments) override;

		private:
			template<typename _>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
			RVariant invoke(RVariant &obj, const ArgumentList &arguments);

			ConstSignature _method;
		};
	}
}

#include "MethodInvoker.hpp"
#include "VoidMethodInvoker.hpp"