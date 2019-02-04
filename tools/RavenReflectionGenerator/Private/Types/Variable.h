#pragma once

#include "CppType.h"

class RClass;

// Class representing a global variable property.
class RVariable : public RCppType
{
public:
	RVariable(
		const RCursor &cursor,
		const FNamespace &currentNamespace,
		RClass* parentClass = nullptr
	);
	virtual ~RVariable(void) { }

	// Check if we should compile this reflection data.
	bool ShouldCompile(void) const;

	const std::string& GetName() const;
	const std::string& GetQualifiedName() const;

private:
	// Is this field constant?
	bool _isConst;

	// Class this field belongs to.
	RClass *_parentClass;

	// Name of this field.
	std::string _name;

	// Qualified of this field.
	std::string _qualifiedName;

	// Type of this field.
	std::string _type;

	// Check if this field accessible by other classes.
	bool IsAccessible(void) const;
};