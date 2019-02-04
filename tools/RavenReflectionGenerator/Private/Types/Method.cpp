#include "Class.h"
#include "Method.h"
#include "Data.h"

RMethod::RMethod(
	const RCursor &cursor,
	const FNamespace &currentNamespace,
	RClass *parent
)
	: RCppType(cursor, currentNamespace)
	, RInvokable(cursor)
	, _isConst(cursor.IsConst())
	, _parentClass(parent)
	, _name(cursor.GetSpelling())
{

}

bool RMethod::ShouldCompile(void) const
{
	return IsAccessible();
}


bool RMethod::IsAccessible(void) const
{
	if (_accessSpecifier != CX_CXXPublic)
		return false;

	// must not be explicitly disabled
	return !_meta.GetFlag(MetaProperty::Disable);
}

std::string RMethod::GetQualifiedSignature(void) const
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

	return (_returnType + "(" + _parentClass->_qualifiedName + "::*)(" + args + ")" + ( _isConst ? " const" : ""));
}