#include "variable.h"
#include "util/util.h"
#include "data.h"

RVariable::RVariable(const RCursor& cursor, const FNamespace& currentNamespace, RClass* parentClass)
	: RCppType(cursor, currentNamespace)
	, _isConst(cursor.GetType().IsConst())
	, _parentClass(parentClass)
	, _name(cursor.GetSpelling())
	, _type(Util::GetQualifiedName(cursor.GetType()))
	, _qualifiedName(currentNamespace.GetQualifiedName(cursor))
{

}

const std::string& RVariable::GetName() const
{
	return _name;
}

const std::string& RVariable::GetQualifiedName() const
{
	return _qualifiedName;
}


bool RVariable::ShouldCompile(void) const
{
	return IsAccessible();
}

bool RVariable::IsAccessible(void) const
{
	return (_accessSpecifier == CX_CXXPublic && !_meta.GetFlag(MetaProperty::Disable) && _bShouldGenerate);
}
