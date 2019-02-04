#include "Parser.h"
#include "Types/Class.h"
#include "Types/Constructor.h"
#include "Types/Enum.h"
#include "Types/Field.h"
#include "Types/Function.h"
#include "Types/Method.h"
#include "Types/Variable.h"
#include <iostream>

RParser::RParser(const FParserOptions& options)
	: _options(options)
	, _index(nullptr)
	, _translationUnit(nullptr)
{
}

RParser::~RParser()
{
	if (_translationUnit)
		clang_disposeTranslationUnit(_translationUnit);

	if (_index)
		clang_disposeIndex(_index);
}

void RParser::Parse()
{
	_index = clang_createIndex(true, _options.bVerbose);

	// create vector of compiler arguments for clang.
	std::vector<const char *> arguments;
	for (auto &argument : _options.compilerArgs)
	{
		arguments.emplace_back(argument.c_str());
	}

	// create translation unit
	_translationUnit = clang_createTranslationUnitFromSourceFile(
		_index,
		_options.inputSourceFile.c_str(),
		static_cast<int>(arguments.size()),
		arguments.data(),
		0,
		nullptr
	);

	// Get cursor for this translation unit.
	auto cursor = clang_getTranslationUnitCursor(_translationUnit);

	FNamespace ns;

	// Parse class types.
	ParseClasses(cursor, ns);

	// Reset namespace for next parsing step.
	ns.Clear();

	// Parse global/static variables.
	ParseVariables(cursor, ns);

	// Reset namespace for next parsing step.
	ns.Clear();

	// Parse global/static functions.
	ParseFunctions(cursor, ns);

	// Reset namespace for next parsing step.
	ns.Clear();

	// Parse enum types.
	ParseEnums(cursor, ns);

	if (_options.bVerbose)
	{
		std::cout << "Parsed " << _sources.size() << " Sources:" << std::endl;

		for (auto pair : _sources)
		{
			std::cout << "[" + pair.first + "]:" << std::endl;
			for (auto cl : pair.second.classes)
			{
				std::cout << "\tClass: " + cl->GetName() + "" << std::endl;
			}
			for (auto en : pair.second.enums)
			{
				std::cout << "\tEnum: " + en->GetName() + "" << std::endl;
			}
			for (auto fu : pair.second.functions)
			{
				std::cout << "\tFunction: " + fu->GetQualifiedName() + "" << std::endl;
			}
			for (auto va : pair.second.variables)
			{
				std::cout << "\tVariable: " + va->GetQualifiedName() + "" << std::endl;
			}
		}
	}
}

void RParser::ParseClasses(const RCursor& cursor, FNamespace& currentNamespace)
{

	for (auto &child : cursor.GetChildren())
	{
		auto kind = child.GetKind();

		// actual definition and a class or struct
		if (child.IsDefinition() &&
			(kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl)
			)
		{
			auto type = std::make_shared<RClass>(child, currentNamespace);

			TRY_ADD_LANGUAGE_TYPE(type, classes);
		}
	
		RECURSE_NAMESPACES(kind, child, ParseClasses, currentNamespace);
	}
}

void RParser::ParseVariables(const RCursor& cursor, FNamespace& currentNamespace)
{
	auto children = cursor.GetChildren();

	for (auto &child : children)
	{
		// skip static globals (hidden)
		if (child.GetStorageClass() == CX_SC_Static)
			continue;

		auto kind = child.GetKind();

		// variable declaration, which is global
		if (kind == CXCursor_VarDecl)
		{
			auto variable = std::make_shared<RVariable>(child, currentNamespace);

			TRY_ADD_LANGUAGE_TYPE(variable, variables);
		}

		RECURSE_NAMESPACES(kind, child, ParseVariables, currentNamespace);
	}
}

void RParser::ParseFunctions(const RCursor& cursor, FNamespace& currentNamespace)
{
	for (auto &child : cursor.GetChildren())
	{
		// skip static globals (hidden)
		if (child.GetStorageClass() == CX_SC_Static)
			continue;

		auto kind = child.GetKind();

		// function declaration, which is global
		if (kind == CXCursor_FunctionDecl)
		{
			auto function = std::make_shared<RFunction>(child, currentNamespace);

			TRY_ADD_LANGUAGE_TYPE(function, functions);
		}

		RECURSE_NAMESPACES(kind, child, ParseFunctions, currentNamespace);
	}
}

void RParser::ParseEnums(const RCursor& cursor, FNamespace& currentNamespace)
{
	for (auto &child : cursor.GetChildren())
	{
		auto kind = child.GetKind();

		// actual definition and an enum
		if (child.IsDefinition() && kind == CXCursor_EnumDecl)
		{
			// anonymous enum if the underlying type display name contains this
			if (child.GetType().GetDisplayName().find("anonymous enum at")
				!= std::string::npos)
			{
				// anonymous enums are just loaded as 
				// globals with each of their values
				for (auto &enumChild : child.GetChildren())
				{
					if (enumChild.GetKind() == CXCursor_EnumConstantDecl)
					{
						auto global = std::make_shared<RVariable>(enumChild, currentNamespace, nullptr);

						TRY_ADD_LANGUAGE_TYPE(global, variables);
					}
				}
			}
			else
			{
				auto enumType = std::make_shared<REnum>(child, currentNamespace);

				TRY_ADD_LANGUAGE_TYPE(enumType, enums);
			}
		}

		RECURSE_NAMESPACES(kind, child, ParseEnums, currentNamespace);
	}
}

void RParser::GenerateFiles()
{
}

void RParser::GenerateHeader(std::string& output) const
{
}

void RParser::GenerateCpp(std::string& output) const
{
}
