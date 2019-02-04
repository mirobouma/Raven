#pragma once

#include "CppType.h"
#include "Invokable.h"

class RClass;

// Class representing a method in a namespace or class.
class RConstructor : public RCppType, RInvokable
{
public:
	RConstructor(
		const RCursor &cursor,
		const FNamespace &currentNamespace,
		RClass* parentClass = nullptr
	);
	virtual ~RConstructor(void) { }

	// Check if we should compile this reflection data.
	bool ShouldCompile(void) const;

private:
	// Class this method belongs to.
	RClass *_parentClass;

	// Check if this field accessible by other classes.
	bool IsAccessible(void) const;
};