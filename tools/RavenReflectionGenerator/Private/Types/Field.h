#pragma once

#include "CppType.h"

class RClass;

// Class representing a field property in a namespace or class.
class RField : public RCppType
{
public:
	RField(
		const RCursor &cursor,
		const FNamespace &currentNamespace,
		RClass* parentClass = nullptr
	);
	virtual ~RField(void) { }

	// Check if we should compile this reflection data.
	bool ShouldCompile(void) const;

private:
	// Is this field constant?
	bool _isConst;

	// Class this field belongs to.
	RClass *_parentClass;

	// Name of this field.
	std::string _name;

	// Type of this field.
	std::string _type;

	// Check if this field accessible by other classes.
	bool IsAccessible(void) const;
};