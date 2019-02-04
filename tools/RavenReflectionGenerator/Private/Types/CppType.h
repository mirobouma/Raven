#pragma once

#include "Cursor.h"
#include "MetaProperties.h"
#include "Namespace.h"

class RCppType
{
public:
	RCppType(const RCursor &cursor, const FNamespace &currentNamespace);
	virtual ~RCppType(void) { }

	// Get a reference to the meta properties.
	const RMetaProperties& GetMeta() const;

	// Get the source file this type is declared in.
	std::string GetSourceFile(void) const;

protected:

	// Meta data properties associated with this type.
	RMetaProperties _meta;

	// determines if the type is enabled in reflection database generation
	bool _bShouldGenerate;

	// Access specifier for this type.
	CX_CXXAccessSpecifier _accessSpecifier;
	
	// Root of this type.
	RCursor _rootCursor;
};