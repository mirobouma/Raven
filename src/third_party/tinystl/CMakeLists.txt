project(tinystl)

set (SourceGroup_PublicIncludes
    "include/TINYSTL/allocator.h"
    "include/TINYSTL/buffer.h"
    "include/TINYSTL/hash.h"
    "include/TINYSTL/hash_base.h"
    "include/TINYSTL/new.h"
    "include/TINYSTL/stddef.h"
    "include/TINYSTL/string.h"
    "include/TINYSTL/string_view.h"
    "include/TINYSTL/traits.h"
    "include/TINYSTL/unordered_map.h"
    "include/TINYSTL/unordered_set.h"
    "include/TINYSTL/vector.h"
)

source_group("Public" FILES ${SourceGroup_PublicIncludes})
set (SOURCE ${SourceGroup_PublicIncludes})

add_library(tinystl INTERFACE)
target_include_directories(tinystl INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include" )
