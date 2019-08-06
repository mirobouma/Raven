#include "enum.h"
#include "data.h"

REnum::FEnumValue::FEnumValue(REnum *parent, const RCursor &cursor)
	: key(cursor.GetDisplayName())
	, value(parent->_qualifiedName + "::" + key)
{

}

REnum::REnum(const RCursor &cursor, const FNamespace &currentNamespace)
	: RCppType(cursor, currentNamespace)
	, _name(cursor.GetType().GetDisplayName())
	, _qualifiedName(_name)
{
	for (auto &child : cursor.GetChildren())
	{
		if (child.GetKind() == CXCursor_EnumConstantDecl)
		{
			RMetaProperties valueMeta(child);

			// don't add disabled values
			if (!valueMeta.GetFlag(MetaProperty::Disable))
				_values.emplace_back(this, child);
		}
	}
}

const std::string& REnum::GetName() const
{
	return _name;
}

const std::string& REnum::GetQualifiedName() const
{
	return _qualifiedName;
}


bool REnum::ShouldCompile(void) const
{
	return IsAccessible();
}

bool REnum::IsAccessible(void) const
{
	return _meta.GetFlag(MetaProperty::Enable);
}