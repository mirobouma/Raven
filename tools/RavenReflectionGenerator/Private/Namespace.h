#pragma once

#include <string>
#include <deque>

class RCursor;
class FNamespace
{
public:
	FNamespace() {}
	~FNamespace() {}

	// Push a namespace onto the stack.
	void PushBack(const std::string nameSpace);

	// Push a namespace onto the stack.
	void PushFront(const std::string nameSpace);

	// Pop a namespace from the stack.
	void Pop();

	// Reset namespace.
	void Clear();

	//  Get the qualified name for the current namespace and append the display name.
	std::string GetQualifiedName(const std::string &displayName) const;

	// Get the qualified name for the current namespace and append the display name of the cursor.
	std::string GetQualifiedName(const RCursor &cursor) const ;

protected:
	// Internal stack for the namespace.
	std::deque<std::string> _stack;
};