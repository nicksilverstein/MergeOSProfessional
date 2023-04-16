
add_definitions(-D__WINESRC__)
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
spec2def(comcat.dll comcat.spec)

list(APPEND SOURCE
    comcat_main.c
    ${CMAKE_CURRENT_BINARY_DIR}/comcat.def)

add_library(comcat MODULE ${SOURCE} version.rc)
set_module_type(comcat win32dll)
add_importlibs(comcat ole32 msvcrt kernel32)
add_cd_file(TARGET comcat DESTINATION mergeos/system32 FOR all)
