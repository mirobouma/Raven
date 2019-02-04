#pragma once

#include "CppType.h"


// Class representing an enum property.
class REnum : public RCppType
{
public:
	struct FEnumValue
	{
		FEnumValue(REnum *parent, const RCursor &cursor);

		std::string key;
		std::string value;
	};

	REnum(
		const RCursor &cursor,
		const FNamespace &currentNamespace
	);

	virtual ~REnum(void) { }

	// Check if we should compile this reflection data.
	bool ShouldCompile(void) const;

	const std::string& GetName() const;
	const std::string& GetQualifiedName() const;

private:
	// Name of this enum.
	std::string _name;

	// Qualified name of this enum.
	std::string _qualifiedName;

	// Type of this enum.
	std::vector<FEnumValue> _values;

	// Check if this enum accessible by other classes.
	bool IsAccessible(void) const;
};