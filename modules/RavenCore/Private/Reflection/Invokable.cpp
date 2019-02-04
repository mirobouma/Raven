#include "Core/Reflection/Invokable/Invokable.h"

namespace Raven
{
	namespace Meta
	{
		RInvokable::RInvokable(const std::string &name)
			: _name(name) { }

		const RInvokableSignature &RInvokable::GetSignature(void) const
		{
			return _signature;
		}

		const std::string &RInvokable::GetName(void) const
		{
			return _name;
		}
	}
}
