/*! \file Enum.h */
#pragma once
#include <type_traits>

/*!
\def ENUM_FLAG_OPERATOR(T,X)
Defines an operator \a X for enum \a T.
*/
#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 

/*!
\def ENUM_FLAG_OPERATOR_ASSIGN(T,X)
Defines an assignment operator \a X for enum \a T.
*/
#define ENUM_FLAG_OPERATOR_ASSIGN(T, X) inline T& operator X= (T& a, T b) { return a = a X b; }

/*!
\def DECL_ENUM_AS_FLAGS(T)
Declares an enum type \a T with bitwise operators so this enum can be easily used for flags.
*/
#define DECL_ENUM_AS_FLAGS(T) \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^) \
ENUM_FLAG_OPERATOR(T,&) \
ENUM_FLAG_OPERATOR_ASSIGN(T,|) \
ENUM_FLAG_OPERATOR_ASSIGN(T,^) \
ENUM_FLAG_OPERATOR_ASSIGN(T,&) 



