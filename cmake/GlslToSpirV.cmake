macro(enable_glsl_to_spirv_compiling trgt)
	if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
		if(WIN32)
			set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
		else()
			set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/bin/glslangValidator")
		endif()
	else()
	  if(WIN32)
			set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
		else()
			set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/bin/glslangValidator")
		endif()
	endif()

	file(GLOB_RECURSE GLSL_SOURCE_FILES
		"${RVN_ROOT_SOURCE}/shaders/*.frag"
		"${RVN_ROOT_SOURCE}/shaders/*.vert"
		"${RVN_ROOT_SOURCE}/shaders/*.comp"
		)

	foreach(GLSL ${GLSL_SOURCE_FILES})
	  get_filename_component(FILE_NAME ${GLSL} NAME)
	  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
	  add_custom_command(
		OUTPUT ${SPIRV}
		COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
		COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
		DEPENDS ${GLSL})
	  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
	endforeach(GLSL)

	add_custom_target(
		Shaders 
		DEPENDS ${SPIRV_BINARY_FILES}
		)
	if(NOT FIX_OUTPUT_PATH)
		add_custom_command(TARGET Shaders POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/shaders/"
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				"${PROJECT_BINARY_DIR}/shaders"
				"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/shaders"
				)
	else()
		add_custom_command(TARGET Shaders POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/"
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				"${PROJECT_BINARY_DIR}/shaders"
				"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders"
				)
	endif()
	add_dependencies(${trgt} Shaders)

endmacro()