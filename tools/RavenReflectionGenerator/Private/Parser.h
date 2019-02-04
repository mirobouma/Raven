#pragma once

#include "Cursor.h"
#include "Namespace.h"
#include <unordered_map>
#include "Source.h"

struct FParserOptions
{
	bool						bForceRebuild = false;
	bool						bVerbose = true;
	std::string					moduleName;
	std::string					inputSourceFile;
	std::string					outputModuleDirectory;
	bool						bUsePCH;
	std::string					PCH;
	std::vector<std::string>	compilerArgs;
};


#define RECURSE_NAMESPACES(kind, cursor, method, ns) \
    if (kind == CXCursor_Namespace)                  \
    {                                                \
        auto displayName = cursor.GetDisplayName( ); \
        if (!displayName.empty( ))                   \
        {                                            \
            ns.PushBack(displayName);                \
            method(cursor, ns);                      \
            ns.Pop();								 \
        }                                            \
    }                                                

#define TRY_ADD_LANGUAGE_TYPE(handle, container)                \
    if (handle->ShouldCompile( ))                               \
    {                                                           \
        auto file = handle->GetSourceFile( );                   \
        _sources[ file ].container.emplace_back( handle );		\
    }                                                           \


class RClass;
class RVariable;
class RFunction;
class REnum;

class RParser
{
public:
	RParser(const FParserOptions& options);
	~RParser();

	void Parse();
	void GenerateFiles();

	void GenerateHeader(std::string &output) const;
	void GenerateCpp(std::string &output) const;

private:
	FParserOptions _options;

	CXIndex _index;
	CXTranslationUnit _translationUnit;

	std::unordered_map<std::string, FSource> _sources;

	void ParseClasses (
		const RCursor &cursor,
		FNamespace &currentNamespace
	);

	void ParseVariables(
		const RCursor &cursor,
		FNamespace &currentNamespace
	);

	void ParseFunctions(
		const RCursor &cursor,
		FNamespace &currentNamespace
	);

	void ParseEnums(
		const RCursor &cursor,
		FNamespace &currentNamespace
	);
};