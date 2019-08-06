#pragma once

#include "cpp_type.h"

#include "constructor.h"
#include "field.h"
#include "variable.h"
#include "method.h"
#include "function.h"
#include <memory>

struct FBaseClass
{
	FBaseClass(const RCursor &cursor);

	std::string name;
};

// Class to describe a class.
class RClass : public RCppType
{
	// to access m_qualifiedName
	friend class RVariable;
	friend class RFunction;
	friend class RMethod;
	friend class RConstructor;
	friend class RField;

public:
	RClass(const RCursor &cursor, const FNamespace &currentNamespace);

	virtual bool ShouldCompile(void) const;

	const std::string& GetName() const;
	const std::string& GetQualifiedName() const;
private:
	// Name of this class.
	std::string _name;

	// Qualified name of this class.
	std::string _qualifiedName;

	template<typename T>
	using SharedPtrVector = std::vector<std::shared_ptr<T>>;

	// Lists of contents of this class.
	SharedPtrVector<FBaseClass>		_baseClasses;
	SharedPtrVector<RConstructor>	_constructors;
	SharedPtrVector<RField>			_fields;
	SharedPtrVector<RVariable>		_staticFields;
	SharedPtrVector<RMethod>		_methods;
	SharedPtrVector<RFunction>		_staticMethods;

	bool IsAccessible(void) const;
};
