
add_definitions(-D__WINESRC__)
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
spec2def(localui.dll localui.spec)

list(APPEND SOURCE
    localui.c
    ${CMAKE_CURRENT_BINARY_DIR}/localui.def)

add_library(localui MODULE ${SOURCE} localui.rc)
set_module_type(localui win32dll)
target_link_libraries(localui wine)
add_importlibs(localui winspool user32 msvcrt kernel32 ntdll)
add_cd_file(TARGET localui DESTINATION mergeos/system32 FOR all)
