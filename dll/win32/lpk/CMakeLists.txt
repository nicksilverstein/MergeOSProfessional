
add_definitions(-DLANGPACK)
include_directories(include)
spec2def(lpk.dll lpk.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    bidi.c
    direction.c
    lpk.c
    stub.c
    ros_lpk.h)

add_library(lpk MODULE
    ${SOURCE}
    lpk.rc
    ${CMAKE_CURRENT_BINARY_DIR}/lpk_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/lpk.def)

set_module_type(lpk win32dll UNICODE ENTRYPOINT LpkDllInitialize 12)
target_link_libraries(lpk wine)
add_importlibs(lpk usp10 version user32 gdi32 kernel32 ntdll)
add_pch(lpk ros_lpk.h SOURCE)
add_cd_file(TARGET lpk DESTINATION mergeos/system32 FOR all)
