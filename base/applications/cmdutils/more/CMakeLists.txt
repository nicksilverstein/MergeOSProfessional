
PROJECT(more)

include_directories(${MERGEOS_SOURCE_DIR}/sdk/lib/conutils)

add_executable(more more.c more.rc)
set_module_type(more win32cui UNICODE)
target_link_libraries(more conutils ${PSEH_LIB})
add_importlibs(more advapi32 msvcrt kernel32)
set_target_properties(more PROPERTIES SUFFIX ".com")
add_cd_file(TARGET more DESTINATION mergeos/system32 FOR all)
