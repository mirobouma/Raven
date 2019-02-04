#include "Core/Reflection/Invokable/Function.h"
#include "Core/Debug/LogManager.h"

namespace Raven
{
	namespace Meta
	{
		RFunction::RFunction(void)
			: RInvokable()
			, _parentType(RType::Invalid())
			, _invoker(nullptr) { }

		const RFunction &RFunction::Invalid(void)
		{
			static RFunction invalid;

			return invalid;
		}

		RType RFunction::GetParentType(void) const
		{
			return _parentType;
		}

		bool RFunction::IsValid(void) const
		{
			return _invoker != nullptr;
		}

		RVariant RFunction::InvokeVariadic(ArgumentList &arguments) const
		{
#if defined(_DEBUG)
			RASSERT_MSG(IsValid(), "Invalid function invocation.");
#endif

			return _invoker->Invoke(arguments);
		}
	}
}
