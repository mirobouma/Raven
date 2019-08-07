#pragma once

#include "core/reflection/metadata/meta_type.h"
#include "core/reflection/type/type.h"

#include "invokable.h"
#include "constructor_invoker.h"

#include <memory>

namespace Raven
{
	namespace Meta
	{
		class RVariant;
		class RArgument;

		class RConstructor
			: public IMetaType
			, public RInvokable
		{
		public:
			RConstructor(void);
			RConstructor(const RConstructor &rhs);
			RConstructor(const RConstructor &&rhs) noexcept;

			RConstructor(
				RType classType,
				RInvokableSignature signature,
				RConstructorInvokerBase *invoker,
				bool isDynamic
			);

			RConstructor &operator=(const RConstructor &&rhs) noexcept;

			static const RConstructor &Invalid(void);

			RType GetClassType(void) const;

			bool IsValid(void) const;
			bool IsDynamic(void) const;

			RVariant InvokeVariadic(const ArgumentList &arguments) const;

			template<typename ...Args>
			RVariant Invoke(Args &&...args) const;

		private:
			bool _isDynamic;
			RType _classType;

			std::shared_ptr<RConstructorInvokerBase> _invoker;
		};
	}
}

#include "constructor.hpp"