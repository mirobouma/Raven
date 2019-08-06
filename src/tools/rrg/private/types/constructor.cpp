#include "class.h"
#include "constructor.h"
#include "data.h"

RConstructor::RConstructor(
	const RCursor &cursor,
	const FNamespace &currentNamespace,
	RClass *parent
)
	: RCppType(cursor, currentNamespace)
	, RInvokable(cursor)
	, _parentClass(parent)
{
}

bool RConstructor::ShouldCompile(void) const
{
	return IsAccessible();
}

bool RConstructor::IsAccessible(void) const
{
	if (_accessSpecifier != CX_CXXPublic)
		return false;

	// must not be explicitly disabled
	return !_meta.GetFlag(MetaProperty::Disable);
}
