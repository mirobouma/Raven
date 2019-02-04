#Project Definition
set(PROJECT_NAME "RRG")

set (SourceGroup_Source
	"Private/Main.cpp"
	"Private/Source.h"
	"Private/Parser.h"
	"Private/Parser.cpp"
	"Private/Cursor.h"
	"Private/Cursor.cpp"
	"Private/Data.h"
	"Private/Namespace.h"
	"Private/Namespace.cpp"
	"Private/MetaProperties.h"
	"Private/MetaProperties.cpp"
	"Private/CursorType.h"
	"Private/CursorType.cpp"
	"Private/Types/CppType.h"
	"Private/Types/CppType.cpp"
	"Private/Types/Class.h"
	"Private/Types/Class.cpp"
	"Private/Types/Constructor.h"
	"Private/Types/Constructor.cpp"
	"Private/Types/Enum.h"
	"Private/Types/Enum.cpp"
	"Private/Types/Field.h"
	"Private/Types/Field.cpp"
	"Private/Types/Function.h"
	"Private/Types/Function.cpp"
	"Private/Types/Method.h"
	"Private/Types/Method.cpp"
	"Private/Types/Invokable.h"
	"Private/Types/Invokable.cpp"
	"Private/Types/Variable.h"
	"Private/Types/Variable.cpp"
	"Private/Util/Util.h"
	"Private/Util/Util.cpp"
	"Private/Util/cxxopts.hpp"
	"Private/Util/mustache.hpp"
	"Private/Main.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
)

find_package(LLVM REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE} ${RAVEN_ICON_RESOURCE_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBCLANG_LIBRARY})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private" ${LLVM_INCLUDE_DIRS})
 

