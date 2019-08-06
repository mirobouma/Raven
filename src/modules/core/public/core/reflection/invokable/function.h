#pragma once

#include "core/reflection/metadata/meta_type.h"
#include "core/reflection/variant/variant.h"
#include "invokable.h"
#include "argument.h"
#include "function_invoker.h"
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

#include "function.hpp"