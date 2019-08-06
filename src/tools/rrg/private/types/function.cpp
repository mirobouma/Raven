#include "class.h"
#include "function.h"
#include "data.h"
#include "util/util.h"

RFunction::RFunction(
	const RCursor &cursor,
	const FNamespace &currentNamespace,
	RClass *parent
)
	: RCppType(cursor, currentNamespace)
	, RInvokable(cursor)
	, _parentClass(parent)
	, _name(cursor.GetSpelling())
	, _qualifiedName(currentNamespace.GetQualifiedName(cursor))
{

}

bool RFunction::ShouldCompile(void) const
{
	return IsAccessible();
}


bool RFunction::IsAccessible(void) const
{
	if (_parentClass && _accessSpecifier != CX_CXXPublic)
		return false;

	return _bShouldGenerate;
}

std::string RFunction::GetQualifiedSignature(void) const
{
	std::string args = "";

	if (_signature.size() == 1)
	{
		args = _signature[0];
	}
	else
	{
		for (uint32_t i = 0; i < _signature.size() - 1; ++i)
		{
			args += _signature[i] + ", ";
		}

		args += _signature[_signature.size() - 1];
	}

	return (_returnType + "(*)(" + args + ")");
}

const std::string& RFunction::GetName() const
{
	return _name;
}

const std::string& RFunction::GetQualifiedName() const
{
	return _qualifiedName;
}
