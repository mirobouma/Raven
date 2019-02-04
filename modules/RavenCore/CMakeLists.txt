#module definition
set(MODULE_NAME "RavenCore")

set (SourceGroup_PublicIncludes
	"Public/Core/Utility/Enum.h"
	"Public/Core/Utility/Hash.h"
	"Public/Core/Utility/Timer.h"

	"Public/Core/Debug/LogManager.h"
	"Public/Core/Debug/Profiler.h"

	"Public/Core/Input/InputManager.h"

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

	"Public/Core/Reflection/Macro.h"
	"Public/Core/Reflection/Reflection.h"
	"Public/Core/Reflection/ReflectionDatabase.h"
	"Public/Core/Reflection/ReflectionDatabase.hpp"

	"Public/Core/Reflection/Metadata/MetaData.h"
	"Public/Core/Reflection/Metadata/MetaData.hpp"
	"Public/Core/Reflection/Metadata/MetaProperty.h"
	"Public/Core/Reflection/Metadata/MetaTraits.h"
	"Public/Core/Reflection/Metadata/MetaType.h"

	"Public/Core/Reflection/Type/Type.h"
	"Public/Core/Reflection/Type/Type.hpp"
	"Public/Core/Reflection/Type/TypeConfig.h"
	"Public/Core/Reflection/Type/TypeID.h"
	"Public/Core/Reflection/Type/TypeData.h"
	"Public/Core/Reflection/Type/TypeData.hpp"
	"Public/Core/Reflection/Type/TypeRegistrant.h"
	"Public/Core/Reflection/Type/TypeRegistrant.hpp"

	"Public/Core/Reflection/Variant/Variant.h"
	"Public/Core/Reflection/Variant/Variant.hpp"
	"Public/Core/Reflection/Variant/VariantPolicy.h"
	"Public/Core/Reflection/Variant/VariantBase.h"
	"Public/Core/Reflection/Variant/VariantContainer.h"
	"Public/Core/Reflection/Variant/VariantContainerArray.h"
	"Public/Core/Reflection/Variant/VariantContainerArray.hpp"
	"Public/Core/Reflection/Variant/VariantContainer.hpp"

	"Public/Core/Reflection/Variant/Wrappers/ObjectWrapper.h"
	"Public/Core/Reflection/Variant/Wrappers/ArrayWrapper.h"
	"Public/Core/Reflection/Variant/Wrappers/ArrayWrapperBase.h"
	"Public/Core/Reflection/Variant/Wrappers/ArrayWrapperContainer.h"
	"Public/Core/Reflection/Variant/Wrappers/ArrayWrapperContainer.hpp"

	"Public/Core/Reflection/Field/Variable.h"
	"Public/Core/Reflection/Field/VariableGetterBase.h"
	"Public/Core/Reflection/Field/VariableGetter.h"
	"Public/Core/Reflection/Field/VariableGetter.hpp"
	"Public/Core/Reflection/Field/VariableSetterBase.h"
	"Public/Core/Reflection/Field/VariableSetter.h"
	"Public/Core/Reflection/Field/VariableSetter.hpp"
	"Public/Core/Reflection/Field/Field.h"
	"Public/Core/Reflection/Field/FieldGetterBase.h"
	"Public/Core/Reflection/Field/FieldGetter.h"
	"Public/Core/Reflection/Field/FieldGetter.hpp"
	"Public/Core/Reflection/Field/FieldSetterBase.h"
	"Public/Core/Reflection/Field/FieldSetter.h"
	"Public/Core/Reflection/Field/FieldSetter.hpp"

	
	"Public/Core/Reflection/Invokable/Invokable.h"
	"Public/Core/Reflection/Invokable/Invokable.hpp"
	"Public/Core/Reflection/Invokable/InvokableConfig.h"
	"Public/Core/Reflection/Invokable/TypeUnpacker.hpp"

	"Public/Core/Reflection/Invokable/Argument.h"
	"Public/Core/Reflection/Invokable/Argument.hpp"
	"Public/Core/Reflection/Invokable/ArgumentConfig.h"

	"Public/Core/Reflection/Invokable/Constructor.h"
	"Public/Core/Reflection/Invokable/Constructor.hpp"
	"Public/Core/Reflection/Invokable/ConstructorInvoker.h"
	"Public/Core/Reflection/Invokable/ConstructorInvoker.hpp"
	"Public/Core/Reflection/Invokable/ConstructorInvokerBase.h"

	"Public/Core/Reflection/Invokable/Destructor.h"
	"Public/Core/Reflection/Invokable/DestructorInvoker.h"
	"Public/Core/Reflection/Invokable/DestructorInvoker.hpp"
	"Public/Core/Reflection/Invokable/DestructorInvokerBase.h"

	"Public/Core/Reflection/Invokable/Function.h"
	"Public/Core/Reflection/Invokable/Function.hpp"
	"Public/Core/Reflection/Invokable/FunctionInvoker.h"
	"Public/Core/Reflection/Invokable/FunctionInvoker.hpp"
	"Public/Core/Reflection/Invokable/FunctionInvokerBase.h"
	"Public/Core/Reflection/Invokable/VoidFunctionInvoker.hpp"

	"Public/Core/Reflection/Invokable/Method.h"
	"Public/Core/Reflection/Invokable/Method.hpp"
	"Public/Core/Reflection/Invokable/MethodInvoker.h"
	"Public/Core/Reflection/Invokable/MethodInvoker.hpp"
	"Public/Core/Reflection/Invokable/MethodInvokerBase.h"
	"Public/Core/Reflection/Invokable/VoidMethodInvoker.hpp"
	

	"Public/Core/Reflection/Enum/Enum.h"
	"Public/Core/Reflection/Enum/EnumBase.h"
	"Public/Core/Reflection/Enum/EnumContainer.h"
	"Public/Core/Reflection/Enum/EnumContainer.hpp"

	"Public/Core/Math/Math.h"
	"Public/Core/Math/Rect.h"
	"Public/Core/Math/Transform.h"
	"Public/Core/Math/Camera.h"

	"Public/Core/Containers/Array.h"
	"Public/Core/Containers/Array.hpp"

	"Public/Core/WindowSystem/WindowSystem.h"
	"Public/Core/Object/Object.h"

)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"Private/Core.cpp"

	"Private/Debug/LogManager.cpp"
	"Private/Debug/Profiler.cpp"

	"Private/Threading/TaskScheduler.cpp"
	"Private/Threading/TaskCounter.cpp"

	"Private/Memory/MemoryUtils.cpp"
	"Private/Memory/LinearAllocator.cpp"
	"Private/Memory/StackAllocator.cpp"
	"Private/Memory/PoolAllocator.cpp"
	"Private/Memory/BuddyAllocator.cpp"

	"Private/Input/InputManager.cpp"

	"Private/Reflection/ReflectionDatabase.cpp"
	"Private/Reflection/Type.cpp"
	"Private/Reflection/Variable.cpp"
	"Private/Reflection/Field.cpp"
	"Private/Reflection/Variant.cpp"
	"Private/Reflection/VariantBase.cpp"
	"Private/Reflection/VariantContainer.cpp"
	"Private/Reflection/Metadata.cpp"
	"Private/Reflection/TypeData.cpp"
	"Private/Reflection/ObjectWrapper.cpp"
	"Private/Reflection/ArrayWrapper.cpp"
	"Private/Reflection/Enum.cpp"
	"Private/Reflection/EnumBase.cpp"
	"Private/Reflection/Invokable.cpp"
	"Private/Reflection/Argument.cpp"
	"Private/Reflection/Constructor.cpp"
	"Private/Reflection/Destructor.cpp"
	"Private/Reflection/Function.cpp"
	"Private/Reflection/Method.cpp"
	"Private/Math/Math.cpp"

	"Private/WindowSystem/WindowSystem.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC RavenHAL gainputstatic glm)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private")

