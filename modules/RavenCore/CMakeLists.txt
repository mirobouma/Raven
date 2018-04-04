#module definition
set(MODULE_NAME "RavenCore")

set (SourceGroup_PublicIncludes
	"Public/Core/Service/Service.h"
	"Public/Core/Service/ServiceProvider.h"
	"Public/Core/Debug/LogManager.h"
	"Public/Core/Threading/Thread.h"
	"Public/Core/Threading/ThreadPool.h"
	"Public/Core/Threading/Fiber.h"
	"Public/Core/Threading/TaskScheduler.h"
	"Public/Core/Threading/AtomicCounter.h"
	"Public/Core/Memory/MemoryUtils.h"
	"Public/Core/Memory/LinearAllocator.h"
	"Public/Core/Memory/StackAllocator.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/Core.cpp"
	"Private/Debug/LogManager.cpp"
	"Private/Service/ServiceProvider.cpp"
	"Private/Threading/TaskScheduler.cpp"
	"Private/Memory/MemoryUtils.cpp"
	"Private/Memory/LinearAllocator.cpp"
	"Private/Memory/StackAllocator.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenMath RavenHAL)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

