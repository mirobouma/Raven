#pragma once

#include "CppType.h"
#include "Invokable.h"

class RClass;

// Class representing a method in a namespace or class.
class RMethod : public RCppType, RInvokable
{
public:
	RMethod(
		const RCursor &cursor,
		const FNamespace &currentNamespace,
		RClass* parentClass = nullptr
	);
	virtual ~RMethod(void) { }

	// Check if we should compile this reflection data.
	bool ShouldCompile(void) const;

private:
	// Is this method constant?
	bool _isConst;

	// Class this method belongs to.
	RClass *_parentClass;

	// Name of this method.
	std::string _name;

	// Check if this field accessible by other classes.
	bool IsAccessible(void) const;

	// Get the signature of this method as a full string.
	std::string GetQualifiedSignature(void) const;
};