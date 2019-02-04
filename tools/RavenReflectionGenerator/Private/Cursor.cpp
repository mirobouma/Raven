/* ----------------------------------------------------------------------------
** Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
**
** Cursor.cpp
** --------------------------------------------------------------------------*/

#include "Cursor.h"
#include "Util/Util.h"

RCursor::RCursor(const CXCursor &handle)
	: _handle(handle) { }

CXCursorKind RCursor::GetKind(void) const
{
	return _handle.kind;
}

RCursor RCursor::GetLexicalParent(void) const
{
	return clang_getCursorLexicalParent(_handle);
}

RCursor RCursor::GetTemplateSpecialization(void) const
{
	return clang_getSpecializedCursorTemplate(_handle);
}

std::string RCursor::GetSpelling(void) const
{
	std::string spelling;

	Util::ToString(clang_getCursorSpelling(_handle), spelling);

	return spelling;
}

std::string RCursor::GetDisplayName(void) const
{
	std::string displayName;

	Util::ToString(clang_getCursorDisplayName(_handle), displayName);

	return displayName;
}

std::string RCursor::GetMangledName(void) const
{
	std::string mangled;

	Util::ToString(clang_Cursor_getMangling(_handle), mangled);

	return mangled;
}

std::string RCursor::GetUSR(void) const
{
	std::string usr;

	Util::ToString(clang_getCursorUSR(_handle), usr);

	return usr;
}

std::string RCursor::GetSourceFile(void) const
{
	auto range = clang_Cursor_getSpellingNameRange(_handle, 0, 0);

	auto start = clang_getRangeStart(range);

	CXFile file;
	unsigned line, column, offset;

	clang_getFileLocation(start, &file, &line, &column, &offset);

	std::string filename;

	Util::ToString(clang_getFileName(file), filename);

	return filename;
}

bool RCursor::IsDefinition(void) const
{
	return clang_isCursorDefinition(_handle) ? true : false;
}

bool RCursor::IsConst(void) const
{
	return clang_CXXMethod_isConst(_handle) ? true : false;
}

bool RCursor::IsStatic(void) const
{
	return clang_CXXMethod_isStatic(_handle) ? true : false;
}

CX_CXXAccessSpecifier RCursor::GetAccessModifier(void) const
{
	return clang_getCXXAccessSpecifier(_handle);
}

CX_StorageClass RCursor::GetStorageClass(void) const
{
	return clang_Cursor_getStorageClass(_handle);
}

RCursorType RCursor::GetType(void) const
{
	return clang_getCursorType(_handle);
}

RCursorType RCursor::GetReturnType(void) const
{
	return clang_getCursorResultType(_handle);
}

RCursorType RCursor::GetTypedefType(void) const
{
	return clang_getTypedefDeclUnderlyingType(_handle);
}

RCursor::CursorList RCursor::GetChildren(void) const
{
	CursorList children;

	auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data)
	{
		auto container = static_cast<CursorList *>(data);

		container->emplace_back(cursor);

		if (cursor.kind == CXCursor_LastPreprocessing)
			return CXChildVisit_Break;

		return CXChildVisit_Continue;
	};

	clang_visitChildren(_handle, visitor, &children);

	return children;
}

void RCursor::VisitChildren(Visitor visitor, void *data)
{
	clang_visitChildren(_handle, visitor, data);
}

unsigned RCursor::GetHash(void) const
{
	return clang_hashCursor(_handle);
}