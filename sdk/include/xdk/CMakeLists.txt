
file(GLOB xdk_headers "*.h")

function(add_xdk_header _template _out)
    get_filename_component(_out_filename ${_out} NAME)
    set(_temp_file ${CMAKE_CURRENT_BINARY_DIR}/${_out_filename})
    add_custom_command(
            OUTPUT ${MERGEOS_BINARY_DIR}/${_out}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_temp_file} ${MERGEOS_BINARY_DIR}/${_out}
            DEPENDS ${_temp_file})

    add_custom_command(
            OUTPUT ${_temp_file}
            COMMAND native-hpp ${_template} ${_temp_file}
            DEPENDS ${_template} ${xdk_headers} native-hpp
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set(xdk_files "${xdk_files};${MERGEOS_BINARY_DIR}/${_out}" PARENT_SCOPE)
endfunction()

add_xdk_header(wdm.template.h sdk/include/ddk/wdm.h)
add_xdk_header(ntddk.template.h sdk/include/ddk/ntddk.h)
add_xdk_header(ntifs.template.h sdk/include/ddk/ntifs.h)
add_xdk_header(devioctl.template.h sdk/include/psdk/devioctl.h)
add_xdk_header(ntdef.template.h sdk/include/psdk/ntdef.h)
add_xdk_header(winnt.template.h sdk/include/psdk/winnt.h)
add_custom_target(xdk DEPENDS ${xdk_files})
