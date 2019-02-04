#include "Field.h"
#include "Util/Util.h"
#include "Data.h"

RField::RField(const RCursor& cursor, const FNamespace& currentNamespace, RClass* parentClass)
	: RCppType(cursor, currentNamespace)
	, _isConst(cursor.GetType().IsConst())
	, _parentClass(parentClass)
	, _name(cursor.GetSpelling())
	, _type(Util::GetQualifiedName(cursor.GetType()))
{

}

bool RField::ShouldCompile(void) const
{
	return IsAccessible();
}

bool RField::IsAccessible(void) const
{
	return (_accessSpecifier == CX_CXXPublic && !_meta.GetFlag(MetaProperty::Disable));
}
