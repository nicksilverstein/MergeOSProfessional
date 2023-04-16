
add_definitions(-D__WINESRC__)
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
spec2def(npptools.dll npptools.spec)

add_library(npptools MODULE
    main.c
    ${CMAKE_CURRENT_BINARY_DIR}/npptools_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/npptools.def)

set_module_type(npptools win32dll)
add_importlibs(npptools msvcrt kernel32 ntdll)
add_cd_file(TARGET npptools DESTINATION mergeos/system32 FOR all)
