
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/fmifs)
include_directories(${MERGEOS_SOURCE_DIR}/sdk/lib/conutils)

add_executable(format format.c format.rc)
set_module_type(format win32cui UNICODE)
target_link_libraries(format conutils ${PSEH_LIB})
add_importlibs(format fmifs msvcrt kernel32 ntdll)
set_target_properties(format PROPERTIES SUFFIX ".com")
add_cd_file(TARGET format DESTINATION mergeos/system32 FOR all)
