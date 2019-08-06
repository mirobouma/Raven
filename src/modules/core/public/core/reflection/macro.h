#pragma once
#include <type_traits>
#include "type/type.h"
#include "metadata/meta_traits.h"

#if defined(__REFLECTION_PARSER__)

#define RMETA(...) __attribute__((annotate(#__VA_ARGS__)))
#define RMETA_OBJECT

#else 

#define RMETA(...) 

#define RAVEN_OBJECT										 \
		Raven::RType GetType(void) const override		 \
        {                                                \
            return typeof( decltype( *this ) );          \
        }                                                \
														 \
        Raven::RObject *Clone(void) const override       \
        {                                                \
            typedef                                      \
            std::remove_const<                           \
                std::remove_reference<                   \
                    decltype( *this )                    \
                >::type                                  \
            >::type ClassType;                           \
            return new ClassType( *this );               \
        }                                                \

#endif
