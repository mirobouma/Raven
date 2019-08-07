#module definition
set(MODULE_NAME "core")

set (SourceGroup_PublicIncludes
	"public/core/application/application.h"

	"public/core/utility/enum.h"
	"public/core/utility/hash.h"
	"public/core/utility/timer.h"

	"public/core/debug/log_manager.h"
	"public/core/debug/profiler.h"

	"public/core/input/input_manager.h"

	"public/core/threading/thread.h"
	"public/core/threading/fiber.h"
	"public/core/threading/task_scheduler.h"
	"public/core/threading/task.h"
	"public/core/threading/task_allocator.h"
	"public/core/threading/task_queue.h"
	"public/core/threading/task_counter.h"

	"public/core/memory/memory_utils.h"
	"public/core/memory/linear_allocator.h"
	"public/core/memory/stack_allocator.h"
	"public/core/memory/pool_allocator.h"
	"public/core/memory/buddy_allocator.h"

	"public/core/reflection/macro.h"
	"public/core/reflection/reflection.h"
	"public/core/reflection/reflection_database.h"
	"public/core/reflection/reflection_database.hpp"

	"public/core/reflection/metadata/metadata.h"
	"public/core/reflection/metadata/metadata.hpp"
	"public/core/reflection/metadata/meta_property.h"
	"public/core/reflection/metadata/meta_traits.h"
	"public/core/reflection/metadata/meta_type.h"

	"public/core/reflection/type/type.h"
	"public/core/reflection/type/type.hpp"
	"public/core/reflection/type/type_config.h"
	"public/core/reflection/type/type_id.h"
	"public/core/reflection/type/type_data.h"
	"public/core/reflection/type/type_data.hpp"
	"public/core/reflection/type/type_registrant.h"
	"public/core/reflection/type/type_registrant.hpp"

	"public/core/reflection/variant/variant.h"
	"public/core/reflection/variant/variant.hpp"
	"public/core/reflection/variant/variant_policy.h"
	"public/core/reflection/variant/variant_base.h"
	"public/core/reflection/variant/variant_container.h"
	"public/core/reflection/variant/variant_container_array.h"
	"public/core/reflection/variant/variant_container_array.hpp"
	"public/core/reflection/variant/variant_container.hpp"

	"public/core/reflection/variant/wrappers/object_wrapper.h"
	"public/core/reflection/variant/wrappers/array_wrapper.h"
	"public/core/reflection/variant/wrappers/array_wrapper_base.h"
	"public/core/reflection/variant/wrappers/array_wrapper_container.h"
	"public/core/reflection/variant/wrappers/array_wrapper_container.hpp"

	"public/core/reflection/field/variable.h"
	"public/core/reflection/field/variable_getter_base.h"
	"public/core/reflection/field/variable_getter.h"
	"public/core/reflection/field/variable_getter.hpp"
	"public/core/reflection/field/variable_setter_base.h"
	"public/core/reflection/field/variable_setter.h"
	"public/core/reflection/field/variable_setter.hpp"
	"public/core/reflection/field/field.h"
	"public/core/reflection/field/field_getter_base.h"
	"public/core/reflection/field/field_getter.h"
	"public/core/reflection/field/field_getter.hpp"
	"public/core/reflection/field/field_setter_base.h"
	"public/core/reflection/field/field_setter.h"
	"public/core/reflection/field/field_setter.hpp"

	
	"public/core/reflection/invokable/invokable.h"
	"public/core/reflection/invokable/invokable.hpp"
	"public/core/reflection/invokable/invokable_config.h"
	"public/core/reflection/invokable/type_unpacker.hpp"

	"public/core/reflection/invokable/argument.h"
	"public/core/reflection/invokable/argument.hpp"
	"public/core/reflection/invokable/argument_config.h"

	"public/core/reflection/invokable/constructor.h"
	"public/core/reflection/invokable/constructor.hpp"
	"public/core/reflection/invokable/constructor_invoker.h"
	"public/core/reflection/invokable/constructor_invoker.hpp"
	"public/core/reflection/invokable/constructor_invoker_base.h"

	"public/core/reflection/invokable/destructor.h"
	"public/core/reflection/invokable/destructor_invoker.h"
	"public/core/reflection/invokable/destructor_invoker.hpp"
	"public/core/reflection/invokable/destructor_invoker_base.h"

	"public/core/reflection/invokable/function.h"
	"public/core/reflection/invokable/function.hpp"
	"public/core/reflection/invokable/function_invoker.h"
	"public/core/reflection/invokable/function_invoker.hpp"
	"public/core/reflection/invokable/function_invoker_base.h"
	"public/core/reflection/invokable/void_function_invoker.hpp"

	"public/core/reflection/invokable/method.h"
	"public/core/reflection/invokable/method.hpp"
	"public/core/reflection/invokable/method_invoker.h"
	"public/core/reflection/invokable/method_invoker.hpp"
	"public/core/reflection/invokable/method_invoker_base.h"
	"public/core/reflection/invokable/void_method_invoker.hpp"
	

	"public/core/reflection/enum/enum.h"
	"public/core/reflection/enum/enum_base.h"
	"public/core/reflection/enum/enum_container.h"
	"public/core/reflection/enum/enum_container.hpp"

	"public/core/math/math.h"
	"public/core/math/rect.h"
	"public/core/math/transform.h"
	"public/core/math/camera.h"

	"public/core/containers/array.h"
	"public/core/containers/array.hpp"

	"public/core/window_system/window_system.h"
	"public/core/object/object.h"
)
source_group("Include" FILES ${SourceGroup_PublicIncludes})

set (SourceGroup_Source
	"private/core.cpp"

	"private/debug/log_manager.cpp"
	"private/debug/profiler.cpp"

	"private/threading/task_scheduler.cpp"
	"private/threading/task_counter.cpp"

	"private/memory/memory_utils.cpp"
	"private/memory/linear_allocator.cpp"
	"private/memory/stack_allocator.cpp"

	"private/input/input_manager.cpp"

	"private/reflection/reflection_database.cpp"
	"private/reflection/type.cpp"
	"private/reflection/variable.cpp"
	"private/reflection/field.cpp"
	"private/reflection/variant.cpp"
	"private/reflection/variant_base.cpp"
	"private/reflection/variant_container.cpp"
	"private/reflection/metadata.cpp"
	"private/reflection/type_data.cpp"
	"private/reflection/object_wrapper.cpp"
	"private/reflection/array_wrapper.cpp"
	"private/reflection/enum.cpp"
	"private/reflection/enum_base.cpp"
	"private/reflection/invokable.cpp"
	"private/reflection/argument.cpp"
	"private/reflection/constructor.cpp"
	"private/reflection/destructor.cpp"
	"private/reflection/function.cpp"
	"private/reflection/method.cpp"
	"private/math/math.cpp"

	"private/window_system/window_system.cpp"
)
source_group("Source" FILES ${SourceGroup_Source})

set (SOURCE
	${SourceGroup_Source}
	${SourceGroup_PublicIncludes}
)

add_library(${MODULE_NAME} STATIC ${SOURCE})
target_link_libraries(${MODULE_NAME} PUBLIC hal sdl2 glm)
target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

