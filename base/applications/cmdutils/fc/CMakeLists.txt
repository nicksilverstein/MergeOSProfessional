include_directories(${MERGEOS_SOURCE_DIR}/sdk/lib/conutils)

add_executable(fc fc.c texta.c textw.c fc.rc)
set_module_type(fc win32cui UNICODE)
target_link_libraries(fc conutils ${PSEH_LIB})
add_importlibs(fc msvcrt shlwapi user32 kernel32)
add_cd_file(TARGET fc DESTINATION mergeos/system32 FOR all)
