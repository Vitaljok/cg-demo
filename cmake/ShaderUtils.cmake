function(add_shaders TARGET_NAME)
    if(NOT ARGN)
        return()
    endif()
    
    set(output_files)
    foreach(shader_file ${ARGN})
        get_filename_component(input_file ${shader_file} ABSOLUTE ) 
        get_filename_component(file_name ${shader_file} NAME)
        set(output_file ${CMAKE_SOURCE_DIR}/assets/shaders/${TARGET_NAME}/${file_name})
        add_custom_command(
            OUTPUT ${output_file}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${input_file} ${output_file}
            DEPENDS ${shader_file}
            COMMENT "Copying shader ${shader_file}"
        )
        list(APPEND output_files ${output_file})
    endforeach()

    add_custom_target(${TARGET_NAME}-shaders ALL
        DEPENDS ${output_files}
        SOURCES ${ARGN})
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}-shaders)
endfunction()