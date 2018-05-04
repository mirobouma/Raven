#include "Platform/Generic/GenericPlatformFileSystem.h"
#include "Core/Debug/LogManager.h"
#include <stdarg.h>
#include <iostream>
#include <fstream>

namespace Raven
{
	std::vector<char> RGenericPlatformFileSystem::ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) 
		{
			RLOG_ERROR("failed to open file!");
		}

		// Get file size because we start at the end of the file.
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		// go to beginning of file and read contents.
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
}