#pragma once
#include "cursor.h"
#include "namespace.h"

class RInvokable
{
public:
	typedef std::vector<std::string> Signature;

	RInvokable(const RCursor &cursor);

protected:
	// return type of this invokable.
	std::string _returnType;

	// Signature of this invokable.
	Signature _signature;
};