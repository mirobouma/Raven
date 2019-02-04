#pragma once
#include "Types/Class.h"
#include "Types/Variable.h"
#include "Types/Function.h"
#include "Types/Enum.h"
#include <memory>

struct FSource {
	std::vector<std::shared_ptr<RClass>> classes;
	std::vector<std::shared_ptr<RVariable>> variables;
	std::vector<std::shared_ptr<RFunction>> functions;
	std::vector<std::shared_ptr<REnum>> enums;
};