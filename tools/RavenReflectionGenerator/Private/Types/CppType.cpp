#include "CppType.h"
#include "Cursor.h"
#include "Data.h"

RCppType::RCppType(
	const RCursor &cursor,
	const FNamespace &currentNamespace
)
	: _meta(cursor)
	, _accessSpecifier(cursor.GetAccessModifier())
	, _rootCursor(cursor)
	, _bShouldGenerate(GetMeta().GetFlag(MetaProperty::Enable))
{

}

const RMetaProperties& RCppType::GetMeta(void) const
{
	return _meta;
}

std::string RCppType::GetSourceFile(void) const
{
	return _rootCursor.GetSourceFile();
}