#include "Core/Reflection/Invokable/Destructor.h"
#include "Core/Reflection/Variant/Variant.h"

#include "Core/Debug/LogManager.h"

namespace Raven
{
	namespace Meta
	{
		namespace
		{
			const auto kDestructorName = "destructor";
		}

		RDestructor::RDestructor(void)
			: RInvokable()
			, _classType(RType::Invalid()) { }

		RDestructor::RDestructor(RType classType, RDestructorInvokerBase *invoker)
			: RInvokable(kDestructorName)
			, _classType(classType)
			, _invoker(invoker) { }

		RType RDestructor::GetClassType(void) const
		{
			return _classType;
		}

		void RDestructor::Invoke(RVariant &instance) const
		{
			RASSERT_MSG(IsValid(),
				"Invalid constructor invoked."
			);

			RASSERT_MSG(_classType == instance.GetType(),
				"Destructor called on incompatible type."
			);

			_invoker->Invoke(instance);

			delete instance._data;

			instance._data = nullptr;
		}

		const RDestructor &RDestructor::Invalid(void)
		{
			static RDestructor invalid;
			return invalid;
		}

		bool RDestructor::IsValid(void) const
		{
			return _invoker != nullptr;
		}
	}
}