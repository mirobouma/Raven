#include "Namespace.h"
#include "Cursor.h"

void FNamespace::PushBack(const std::string name)
{
	_stack.push_back(name);
}

void FNamespace::PushFront(const std::string name)
{
	_stack.push_front(name);
}

void FNamespace::Clear()
{
	_stack.clear();
}

void FNamespace::Pop()
{
	_stack.pop_back();
}

std::string FNamespace::GetQualifiedName(const std::string &displayName) const
{
	std::string name = "";

	for (std::string ns : _stack)
	{
		name += ns + "::";
	}

	return name + displayName;
}

std::string FNamespace::GetQualifiedName(const RCursor &cursor) const
{
	return GetQualifiedName(cursor.GetSpelling());
}