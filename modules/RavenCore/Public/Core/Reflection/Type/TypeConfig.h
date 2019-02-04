
#pragma once

#include "TypeID.h"
#include "Core/Reflection/Metadata/MetaTraits.h"

#include <type_traits>


#define typeidof(expr)                                            \
    Raven::Meta::TypeIdOf<                                        \
        Raven::Meta::CleanedType<                                \
            typename Raven::Meta::RemoveArray<expr>::type \
        >                                                         \
    >::ID                                                         \

#define typeof(expr)                              \
    Raven::Meta::RType(                           \
        typeidof( expr ),                         \
        Raven::Meta::IsArray<expr>::value \
    )                                             \

#define decltypeof(expr) typeof( decltype( expr ) )

namespace Raven
{
	namespace Meta
	{
		template<typename T>
		using CleanedType =
			typename std::remove_cv<
			typename std::remove_reference< T >::type
			>::type;
	}
}