
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
add_definitions(-D__WINESRC__)
spec2def(url.dll url.spec)

list(APPEND SOURCE
    url_main.c
    ${CMAKE_CURRENT_BINARY_DIR}/url_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/url.def)

add_library(url MODULE ${SOURCE})
set_module_type(url win32dll)
target_link_libraries(url wine)
add_importlibs(url shell32 shlwapi msvcrt kernel32 ntdll)
add_cd_file(TARGET url DESTINATION mergeos/system32 FOR all)
