
include_directories(
    ${MERGEOS_SOURCE_DIR}/win32ss/user/winsrv/concfg)

spec2def(console.dll console.spec)

list(APPEND SOURCE
    console.c
    options.c
    font.c
    layout.c
    colors.c
    utils.c
    console.h)

add_rc_deps(console.rc ${CMAKE_CURRENT_SOURCE_DIR}/res/terminal.ico)

add_library(console MODULE
    ${SOURCE}
    console.rc
    ${CMAKE_CURRENT_BINARY_DIR}/console.def)

set_module_type(console cpl UNICODE)
set_target_properties(console PROPERTIES SUFFIX ".dll")

target_link_libraries(console concfg)
add_importlibs(console advapi32 user32 gdi32 comctl32 msvcrt kernel32 ntdll)
add_pch(console console.h SOURCE)
add_cd_file(TARGET console DESTINATION mergeos/system32 FOR all)
