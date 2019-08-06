#pragma once

#include "core/reflection/metadata/meta_type.h"
#include "core/reflection/invokable/invokable.h"
#include "core/reflection/variant/variant.h"
#include "core/reflection/invokable/argument.h"
#include "core/reflection/invokable/method_invoker.h"

#include <vector>
#include <unordered_map>
#include <functional>

namespace Raven
{
	namespace Meta
	{
		class RMethod
			: public IMetaType
			, public RInvokable
		{
		public:
			RMethod(void);

			template<
				class ClassType,
				typename ReturnType,
				typename ...ArgTypes>
				RMethod(
					const std::string &name,
					ReturnType(ClassType::*method)(ArgTypes...)
				);

			template<
				class ClassType,
				typename ReturnType,
				typename ...ArgTypes>
				RMethod(
					const std::string &name,
					ReturnType(ClassType::*method)(ArgTypes...) const
				);

			static const RMethod &Invalid(void);

			RType GetClassType(void) const;

			bool IsValid(void) const;
			bool IsConst(void) const;

			RVariant Invoke(RVariant &instance, ArgumentList &arguments) const;

			template<typename ...Args>
			RVariant Invoke(RVariant &instance, Args &&...args) const;

		private:
			bool _isConst;
			RType _classType;

			std::shared_ptr<RMethodInvokerBase> _invoker;
		};
	}
}

#include "method.hpp"