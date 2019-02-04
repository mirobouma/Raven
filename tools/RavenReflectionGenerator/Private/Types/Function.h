#pragma once

#include "CppType.h"
#include "Invokable.h"

class RClass;

// Class representing a function in a namespace or class.
class RFunction : public RCppType, RInvokable
{
public:
	RFunction(
		const RCursor &cursor,
		const FNamespace &currentNamespace,
		RClass* parentClass = nullptr
	);
	virtual ~RFunction(void) { }

	// Check if we should compile this reflection data.
	bool ShouldCompile(void) const;

	const std::string& GetName() const;
	const std::string& GetQualifiedName() const;

	// Get the signature of this function as a full string.
	std::string GetQualifiedSignature(void) const;

private:

	// Class this function belongs to.
	RClass *_parentClass;

	// Name of this function.
	std::string _name;

	// qualified name of this function.
	std::string _qualifiedName;

	// Check if this function accessible by other classes.
	bool IsAccessible(void) const;
};