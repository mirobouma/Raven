#include "Class.h"
#include "Data.h"


bool IsNativeType(const std::string &qualifiedName)
{
	return std::find(
		gNativeTypes.begin(),
		gNativeTypes.end(),
		qualifiedName
	) != gNativeTypes.end();
}

FBaseClass::FBaseClass(const RCursor &cursor)
	: name(cursor.GetType().GetCanonicalType().GetDisplayName())
{}

RClass::RClass(const RCursor &cursor, const FNamespace &currentNamespace)
	: RCppType(cursor, currentNamespace)
	, _name(cursor.GetDisplayName())
	, _qualifiedName(cursor.GetType().GetDisplayName())
{
	for (auto &child : cursor.GetChildren())
	{
		switch (child.GetKind())
		{
		case CXCursor_CXXBaseSpecifier:
		{
			auto baseClass = new FBaseClass(child);

			_baseClasses.emplace_back(baseClass);

			// automatically enable the type if not explicitly disabled
			if (IsNativeType(baseClass->name))
				_bShouldGenerate = !_meta.GetFlag(MetaProperty::Disable);
		}
		break;
		// constructor
		case CXCursor_Constructor:
			_constructors.emplace_back(
				new RConstructor(child, currentNamespace, this)
			);
			break;
			// field
		case CXCursor_FieldDecl:
			_fields.emplace_back(
				new RField(child, currentNamespace, this)
			);
			break;
			// static field
		case CXCursor_VarDecl:
			_staticFields.emplace_back(
				new RVariable(child, FNamespace(), this)
			);
			break;
			// method / static method
		case CXCursor_CXXMethod:
			if (child.IsStatic())
			{
				_staticMethods.emplace_back(
					new RFunction(child, FNamespace(), this)
				);
			}
			else
			{
				_methods.emplace_back(
					new RMethod(child, currentNamespace, this)
				);
			}
			break;
		default:
			break;
		}
	}
}

bool RClass::ShouldCompile(void) const
{
	return IsAccessible() && !IsNativeType(_qualifiedName);
}

const std::string& RClass::GetName() const
{
	return _name;
}

const std::string& RClass::GetQualifiedName() const
{
	return _qualifiedName;
}

bool RClass::IsAccessible(void) const
{
	return _bShouldGenerate;
}