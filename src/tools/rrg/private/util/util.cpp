#include "util.h"
#include "cursor.h"

void Util::ToString(const CXString &str, std::string &output)
{
	auto cstr = clang_getCString(str);

	output = cstr;

	clang_disposeString(str);
}

std::string Util::GetQualifiedName(const RCursorType &type)
{
	// If this is not a typedef, return the display name.
	if (type.GetKind() != CXType_Typedef)
		return type.GetDisplayName();

	auto declaration = type.GetDeclaration();
	auto parent = declaration.GetLexicalParent();

	FNamespace parentNamespace;

	// walk up to the root namespace
	while (parent.GetKind() == CXCursor_Namespace)
	{
		parentNamespace.PushFront(parent.GetDisplayName());
		parent = parent.GetLexicalParent();
	}

	return parentNamespace.GetQualifiedName(type.GetDisplayName());
}