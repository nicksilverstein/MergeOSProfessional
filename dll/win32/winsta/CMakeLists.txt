
spec2def(winsta.dll winsta.spec)

include_directories(BEFORE ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)

list(APPEND SOURCE
    logon.c
    main.c
    misc.c
    query.c
    security.c
    server.c
    ws.c
    winsta.h)

add_library(winsta MODULE
    ${SOURCE}
    winsta.rc
    ${CMAKE_CURRENT_BINARY_DIR}/winsta.def)

set_module_type(winsta win32dll)
target_link_libraries(winsta wine)
add_importlibs(winsta msvcrt kernel32 ntdll)
add_pch(winsta winsta.h SOURCE)
add_cd_file(TARGET winsta DESTINATION mergeos/system32 FOR all)
