#include <iostream>
#include "Util/cxxopts.hpp"
#include "Parser.h"
#include "Data.h"
#include "Util/Util.h"

constexpr auto RRG_VERSION = "1.0.0";


void missing_required_arg(const char* arg)
{
	std::cout << "Required option \"" << arg << "\" has not been defined, please re-run the tool using the correct arguments." << std::endl;
	exit(1);
}

int main(int argc, char* argv[])
{
	try
	{
		cxxopts::Options options("RavenReflectionTool",
			"RavenReflectionTool - The commandline interface to header reflection generation.\n");

		options.add_options()
			("h,help", "display this help and exit")
			("version", "display version information and exit")
			("v,verbose", "be verbose when parsing");

		options.add_options("compilation")
			("module-name", "(required) name of the module to reflect data for", cxxopts::value<std::string>())
			("module-header", "(required) path to the module header which includes headers to generate reflection data", cxxopts::value<std::string>())
			("out-directory", "(required) set the output directory for this module's generated reflection data", cxxopts::value<std::string>())
			("I,include_directory", "(optional) add directory to include search path", cxxopts::value<std::vector<std::string>>())
			("D,macro_definition", "(optional) define a macro on the command line", cxxopts::value<std::vector<std::string>>())
			("pch", "(optional) set a precompiled header which this module uses", cxxopts::value<std::string>())
			("force-rebuild", "(optional) forces a refresh of the generated code", cxxopts::value<bool>());

		// clang-format on
		auto results = options.parse(argc, argv);

		if (results.count("help"))
		{
			std::cout << options.help({ "", "compilation" }) << '\n';
		}
		else if (results.count("version"))
		{
			std::string clangVer;
			Util::ToString(clang_getClangVersion(), clangVer);

			std::cout << "Raven Reflection Generator version " << RRG_VERSION << "\n";
			std::cout << "Using libclang version " << clangVer << '\n';
		}
		else
		{
			// Create FParserOptions structure.
			FParserOptions p_options;
			p_options.compilerArgs = {
				"-x",
				"c++",
				"-D__REFLECTION_PARSER__"
			};

			if (results.count("module-name"))
			{
				p_options.moduleName = results["module-name"].as<std::string>();
			}
			else
			{
				missing_required_arg("module-name");
			}

			if (results.count("module-header"))
			{
				p_options.inputSourceFile = results["module-header"].as<std::string>();
			}
			else
			{
				missing_required_arg("out-directory");
			}

			if (results.count("out-directory"))
			{
				p_options.outputModuleDirectory = results["out-directory"].as<std::string>();
			}
			else
			{
				missing_required_arg("out-directory");
			}

			if (results.count("force-rebuild"))
			{
				p_options.bForceRebuild = true;
			}
			else
			{
				p_options.bForceRebuild = false;
			}

			if (results.count("include_directory") > 0)
			{
				for (auto& include : results["include_directory"].as<std::vector<std::string>>())
				{
					p_options.compilerArgs.push_back("-I" + include);
				}
			}

			if (results.count("macro_definition") > 0)
			{
				for (auto& def : results["macro_definition"].as<std::vector<std::string>>())
				{
					p_options.compilerArgs.push_back("-D" + def);
				}
			}

			if (results.count("pch"))
			{
				p_options.bUsePCH = true;
				p_options.PCH = results["pch"].as<std::string>();
			}

			// Create parser object.
			RParser parser(p_options);

			std::cout << "Parsing headers for module \"" << p_options.moduleName << "\"..." << std::endl;

			try
			{
				// Parse header files 
				parser.Parse();

				// Generate code based on parsed AST.
				parser.GenerateFiles();
			}
			catch (std::exception &e)
			{
				std::cout << "Error parsing headers for module \"" << p_options.moduleName << "\": " << e.what() << std::endl;
				exit(1);
			}
		}
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cout << "Error parsing options: " << e.what() << std::endl;
		exit(1);
	}
	return 0;
}
