#pragma once
#include "types/class.h"
#include "types/variable.h"
#include "types/function.h"
#include "types/enum.h"
#include <memory>

struct FSource {
	std::vector<std::shared_ptr<RClass>> classes;
	std::vector<std::shared_ptr<RVariable>> variables;
	std::vector<std::shared_ptr<RFunction>> functions;
	std::vector<std::shared_ptr<REnum>> enums;
};