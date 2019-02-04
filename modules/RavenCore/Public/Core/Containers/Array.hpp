#pragma once

namespace Raven
{
	template<typename T>
	TArray<T>::TArray(void) { }

	template<typename T>
	TArray<T>::TArray(const std::vector<T> &rhs)
		: std::vector<T>(rhs) { }

	template<typename T>
	TArray<T>::TArray(const std::vector<T> &&rhs)
		: std::vector<T>(std::move(rhs)) { }

	template<typename T>
	TArray<T>::TArray(const std::initializer_list<T> &rhs)
		: std::vector<T>(rhs) { }

	template<typename T>
	TArray<T>::TArray(const std::initializer_list<T> &&rhs)
		: std::vector<T>(std::move(rhs)) { }
}