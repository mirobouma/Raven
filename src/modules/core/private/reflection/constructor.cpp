#include "core/reflection/invokable/constructor.h"
#include "core/reflection/invokable/invokable.h"
#include "core/reflection/type/type.h"

#include "core/debug/log_manager.h"

namespace Raven
{
	namespace Meta
	{
		namespace
		{
			const auto kConstructorName = "constructor";
		}

		RConstructor::RConstructor(void)
			: RInvokable()
			, _isDynamic(false)
			, _classType(InvalidTypeID)
			, _invoker(nullptr) { }

		RConstructor::RConstructor(const RConstructor &rhs)
			: RInvokable(kConstructorName)
			, _isDynamic(rhs._isDynamic)
			, _classType(rhs._classType)
			, _invoker(rhs._invoker)
		{
			_signature = rhs._signature;
		}

		RConstructor::RConstructor(const RConstructor &&rhs) noexcept
			: RInvokable(kConstructorName)
			, _isDynamic(rhs._isDynamic)
			, _classType(rhs._classType)
			, _invoker(std::move(rhs._invoker))
		{
			_signature = std::move(rhs._signature);
		}

		RConstructor::RConstructor(
			RType classType,
			RInvokableSignature signature,
			RConstructorInvokerBase *invoker,
			bool isDynamic
		)
			: RInvokable(kConstructorName)
			, _isDynamic(isDynamic)
			, _classType(classType)
			, _invoker(invoker)
		{
			_signature = signature;
		}

		RConstructor &RConstructor::operator=(const RConstructor &&rhs)
		{
			_isDynamic = rhs._isDynamic;
			_classType = rhs._classType;
			_invoker = std::move(rhs._invoker);

			_signature = std::move(rhs._signature);

			return *this;
		}

		const RConstructor &RConstructor::Invalid(void)
		{
			static RConstructor invalid;
			return invalid;
		}

		RType RConstructor::GetClassType(void) const
		{
			return _classType;
		}

		bool RConstructor::IsValid(void) const
		{
			return _invoker != nullptr;
		}

		bool RConstructor::IsDynamic(void) const
		{
			return _isDynamic;
		}

		RVariant RConstructor::InvokeVariadic(const ArgumentList &arguments) const
		{
			RASSERT(IsValid(), "Invalid constructor invoked.");

			return _invoker->Invoke(arguments);
		}
	}
}
