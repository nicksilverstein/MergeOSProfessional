
spec2def(esent.dll esent.spec)

list(APPEND SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/esent_stubs.c)

add_library(esent MODULE
    ${SOURCE}
    ${CMAKE_CURRENT_BINARY_DIR}/esent.def)

set_module_type(esent win32dll)
add_importlibs(esent msvcrt kernel32 ntdll)
add_cd_file(TARGET esent DESTINATION mergeos/system32 FOR all)
