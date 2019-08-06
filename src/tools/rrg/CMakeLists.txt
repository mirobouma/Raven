#Project Definition
set(PROJECT_NAME "rrg")

set (SourceGroup_Source
	"private/main.cpp"
	"private/source.h"
	"private/parser.h"
	"private/parser.cpp"
	"private/cursor.h"
	"private/cursor.cpp"
	"private/data.h"
	"private/namespace.h"
	"private/namespace.cpp"
	"private/meta_properties.h"
	"private/meta_properties.cpp"
	"private/cursor_type.h"
	"private/cursor_type.cpp"
	"private/types/cpp_type.h"
	"private/types/cpp_type.cpp"
	"private/types/class.h"
	"private/types/class.cpp"
	"private/types/constructor.h"
	"private/types/constructor.cpp"
	"private/types/enum.h"
	"private/types/enum.cpp"
	"private/types/field.h"
	"private/types/field.cpp"
	"private/types/function.h"
	"private/types/function.cpp"
	"private/types/method.h"
	"private/types/method.cpp"
	"private/types/invokable.h"
	"private/types/invokable.cpp"
	"private/types/variable.h"
	"private/types/variable.cpp"
	"private/util/util.h"
	"private/util/util.cpp"
	"private/util/cxxopts.hpp"
	"private/util/mustache.hpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
)

find_package(LLVM REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE} ${RAVEN_ICON_RESOURCE_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBCLANG_LIBRARY})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private" ${LLVM_INCLUDE_DIRS})
 

