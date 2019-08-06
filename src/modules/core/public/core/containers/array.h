#pragma once
#include <vector>

namespace Raven
{
	template<typename T>
	class TArray : public std::vector<T>
	{
	public:
		TArray(void);
		TArray(const std::vector<T> &rhs);
		TArray(const std::vector<T> &&rhs);
		TArray(const std::initializer_list<T> &rhs);
		TArray(const std::initializer_list<T> &&rhs);
	};
}

#include "Array.hpp"