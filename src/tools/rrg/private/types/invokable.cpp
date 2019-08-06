#include "invokable.h"
#include "util/util.h"

RInvokable::RInvokable(const RCursor &cursor)
	: _returnType(Util::GetQualifiedName(cursor.GetReturnType()))
{
	auto type = cursor.GetType();
	unsigned count = type.GetArgumentCount();

	// Parse signature of this invokable type.
	for (unsigned i = 0; i < count; ++i)
	{
		auto argument = type.GetArgument(i);

		_signature.emplace_back(
			Util::GetQualifiedName(argument)
		);
	}
}