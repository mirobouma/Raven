#pragma once
#include <type_traits>


/*! 
Defines an operator \a X for enum \a T.
*/
#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 

/*!
Declares an enum type \a T with bitwise operators so this enum can be easily used for flags.
*/
#define ENUM_FLAGS(T) \
enum class T; \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^) \
ENUM_FLAG_OPERATOR(T,&) \
enum class T