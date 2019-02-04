#pragma once

#include "Invokable.h"
#include "DestructorInvoker.h"

namespace Raven
{
	namespace Meta
	{
		class RDestructor : public RInvokable
		{
		public:
			RDestructor(void);
			RDestructor(RType classType, RDestructorInvokerBase *invoker);

			static const RDestructor &Invalid(void);

			RType GetClassType(void) const;

			bool IsValid(void) const;

			void Invoke(RVariant &instance) const;

		private:
			RType _classType;
			std::shared_ptr<RDestructorInvokerBase> _invoker;
		};
	}
}