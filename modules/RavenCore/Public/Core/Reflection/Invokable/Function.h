#pragma once

#include "Core/Reflection/Metadata/MetaType.h"
#include "Core/Reflection/Variant/Variant.h"
#include "Invokable.h"
#include "Argument.h"
#include "FunctionInvoker.h"
#include <functional>

namespace Raven
{
	namespace Meta
	{
		class RFunction
			: public IMetaType
			, public RInvokable
		{
		public:
			typedef std::function<RVariant(ArgumentList&)> Invoker;

			RFunction();

			template<typename ReturnType, typename ...ArgTypes>
			RFunction(
				const std::string &name,
				ReturnType(*function)(ArgTypes...),
				RType parentType = RType::Invalid()
			);

			static const RFunction &Invalid(void);

			RType GetParentType(void) const;

			bool IsValid(void) const;

			RVariant InvokeVariadic(ArgumentList &arguments) const;

			template<typename ...Args>
			RVariant Invoke(Args &&...args) const;

		private:
			RType _parentType;

			std::shared_ptr<RFunctionInvokerBase> _invoker;
		};
	}
}

#include "Function.hpp"