function(parse_reflection module_name module_header)

target_include_directories(${module_name} PUBLIC "${RVN_ROOT_SOURCE}/generated/${module_name}")

ADD_CUSTOM_TARGET(${module_name}_reflection
	COMMAND $<TARGET_FILE:rrg> --module-name "${module_name}" --module-header "${module_header}" --out-directory "${RVN_ROOT_SOURCE}/generated/${module_name}"	COMMAND $<TARGET_FILE:rrg> --module-name "${module_name}" --module-header "${module_header}" --out-directory "${RVN_ROOT_SOURCE}/generated/${module_name}" $<1:--include_directory\t$<JOIN:$<TARGET_PROPERTY:core,INCLUDE_DIRECTORIES>,\t--include_directory\t>>
 $<1:--include_directory\t$<JOIN:$<TARGET_PROPERTY:core,INCLUDE_DIRECTORIES>,\t--include_directory\t>>
	COMMENT "Generating reflection data for module ${module_name}"
)
    
add_dependencies(${module_name} ${module_name}_reflection)
endfunction()
