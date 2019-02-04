#include "CursorType.h"
#include "Cursor.h"
#include "Util/Util.h"

RCursorType::RCursorType(const CXType &handle)
	: _handle(handle)
{}

std::string RCursorType::GetDisplayName(void) const
{
	std::string displayName;

	Util::ToString(clang_getTypeSpelling(_handle), displayName);

	return displayName;
}

int RCursorType::GetArgumentCount(void) const
{
	return clang_getNumArgTypes(_handle);
}

RCursorType RCursorType::GetArgument(unsigned index) const
{
	return clang_getArgType(_handle, index);
}

RCursorType RCursorType::GetCanonicalType(void) const
{
	return clang_getCanonicalType(_handle);
}

RCursor RCursorType::GetDeclaration(void) const
{
	return clang_getTypeDeclaration(_handle);
}

CXTypeKind RCursorType::GetKind(void) const
{
	return _handle.kind;
}

bool RCursorType::IsConst(void) const
{
	return clang_isConstQualifiedType(_handle) ? true : false;
}