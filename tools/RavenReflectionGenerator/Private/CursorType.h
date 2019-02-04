#pragma once

#include <string>
#include "clang-c/Index.h"

class RCursor;
class RCursorType
{
public:
	RCursorType(const CXType &handle);

	// Get display name for this cursor type.
	std::string GetDisplayName(void) const;

	// Get amount of arguments.
	int GetArgumentCount(void) const;

	// Get argument and index.
	RCursorType GetArgument(unsigned index) const;

	// Get the canonical type for this cursor.
	RCursorType GetCanonicalType(void) const;

	// Get cursor for the declaration of this type declaration.
	RCursor GetDeclaration(void) const;

	// Get cursor type kind.
	CXTypeKind GetKind(void) const;

	// Returns of this type is constant.
	bool IsConst(void) const;

private:
	CXType _handle;
};