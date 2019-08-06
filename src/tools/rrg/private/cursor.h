#pragma once

#include <string>
#include <vector>
#include "cursor_type.h"

class RCursor
{
public:
	typedef std::vector<RCursor> CursorList;
	typedef CXCursorVisitor Visitor;

	RCursor(const CXCursor &handle);

	CXCursorKind GetKind(void) const;

	RCursor GetLexicalParent(void) const;
	RCursor GetTemplateSpecialization(void) const;

	std::string GetSpelling(void) const;
	std::string GetDisplayName(void) const;
	std::string GetMangledName(void) const;
	std::string GetUSR(void) const;

	std::string GetSourceFile(void) const;

	bool IsDefinition(void) const;
	bool IsConst(void) const;
	bool IsStatic(void) const;

	CX_CXXAccessSpecifier GetAccessModifier(void) const;
	CX_StorageClass GetStorageClass(void) const;

	RCursorType GetType(void) const;
	RCursorType GetReturnType(void) const;
	RCursorType GetTypedefType(void) const;

	CursorList GetChildren(void) const;
	void VisitChildren(Visitor visitor, void *data = nullptr);

	unsigned GetHash(void) const;
private:
	CXCursor _handle;
};