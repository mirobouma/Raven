#module definition
set(MODULE_NAME "RavenCore")

set (SourceGroup_PublicIncludes
	"Public/Core/Service/Service.h"
	"Public/Core/Service/ServiceProvider.h"
	"Public/Core/Utility/Enum.h"
	"Public/Core/Utility/Hash.h"
	"Public/Core/Debug/LogManager.h"
	"Public/Core/Debug/Profiler.h"
	"Public/Core/Threading/Thread.h"
	"Public/Core/Threading/Fiber.h"
	"Public/Core/Threading/TaskScheduler.h"
	"Public/Core/Threading/Task.h"
	"Public/Core/Threading/TaskAllocator.h"
	"Public/Core/Threading/TaskQueue.h"
	"Public/Core/Threading/TaskCounter.h"
	"Public/Core/Memory/MemoryUtils.h"
	"Public/Core/Memory/LinearAllocator.h"
	"Public/Core/Memory/StackAllocator.h"
	"Public/Core/Memory/PoolAllocator.h"
	"Public/Core/Memory/BuddyAllocator.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/Core.cpp"
	"Private/Debug/LogManager.cpp"
	"Private/Debug/Profiler.cpp"
	"Private/Service/ServiceProvider.cpp"
	"Private/Threading/TaskScheduler.cpp"
	"Private/Threading/TaskCounter.cpp"
	"Private/Memory/MemoryUtils.cpp"
	"Private/Memory/LinearAllocator.cpp"
	"Private/Memory/StackAllocator.cpp"
	"Private/Memory/PoolAllocator.cpp"
	"Private/Memory/BuddyAllocator.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenMath RavenHAL)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

