
list(APPEND TEXTFILES
    textfiles/ExtraLargeNormal.INI
    textfiles/LargeFontsNormal.INI
    textfiles/NormalNormal.INI)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/textfiles)
foreach(_file ${TEXTFILES})
    get_filename_component(_file_name ${_file} NAME_WE)
    get_filename_component(_subdir ${_file} DIRECTORY)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_subdir})
    set(_converted_file ${CMAKE_CURRENT_BINARY_DIR}/${_subdir}/${_file_name}_utf16.INI)
    add_custom_command(OUTPUT ${_converted_file}
                       COMMAND native-utf16le ${CMAKE_CURRENT_SOURCE_DIR}/${_file} ${_converted_file}
                       DEPENDS native-utf16le ${CMAKE_CURRENT_SOURCE_DIR}/${_file})
    list(APPEND _converted_files ${_converted_file})
endforeach()

set_source_files_properties(mizu.rc PROPERTIES OBJECT_DEPENDS "${_converted_files}")
add_library(mizu.msstyles MODULE mizu.rc)
set_module_type(mizu.msstyles module)
set_target_properties(mizu.msstyles PROPERTIES SUFFIX "")
add_cd_file(TARGET mizu.msstyles DESTINATION mergeos/Resources/Themes/Mizu FOR all)
