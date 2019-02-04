#include "Core/Reflection/Invokable/Method.h"
#include "Core/Debug/LogManager.h"

namespace Raven
{
	namespace Meta
	{
		RMethod::RMethod(void)
			: RInvokable()
			, _isConst(true)
			, _classType(RType::Invalid())
			, _invoker(nullptr) { }

		const RMethod &RMethod::Invalid(void)
		{
			static RMethod invalid;

			return invalid;
		}

		RType RMethod::GetClassType(void) const
		{
			return _classType;
		}

		bool RMethod::IsValid(void) const
		{
			return _invoker != nullptr;
		}

		bool RMethod::IsConst(void) const
		{
			return _isConst;
		}

		RVariant RMethod::Invoke(
			RVariant &instance,
			ArgumentList &arguments
		) const
		{
#if defined(_DEBUG)

			RASSERT(IsValid(),
				"Invalid RMethod invoked."
			);

			RASSERT(!(instance.IsConst() && !_isConst),
				"Non-const RMethod invoked on const object."
			);

			RASSERT(instance.GetType() == _classType,
				"Incompatible RMethod invoked with instance."
			);
#endif
			return _invoker->Invoke(instance, arguments);
		}
	}
}
