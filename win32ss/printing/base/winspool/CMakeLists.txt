
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl)
add_rpc_files(client ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/winspool.idl)
spec2def(winspool.drv winspool.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    ../marshalling.c
    devmode.c
    forms.c
    jobs.c
    main.c
    monitors.c
    ports.c
    printerdata.c
    printerdrivers.c
    printers.c
    printprocessors.c
    printproviders.c
    spoolfile.c
    utils.c
    ${CMAKE_CURRENT_BINARY_DIR}/winspool_c.c)

list(APPEND PCH_SKIP_SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/winspool_stubs.c)

add_library(winspool MODULE
    ${SOURCE}
    ${PCH_SKIP_SOURCE}
    winspool.rc
    ${CMAKE_CURRENT_BINARY_DIR}/winspool.def)

set_target_properties(winspool PROPERTIES SUFFIX ".drv")
set_module_type(winspool win32dll UNICODE)
target_link_libraries(winspool wine ${PSEH_LIB})
add_importlibs(winspool advapi32 gdi32 user32 rpcrt4 msvcrt kernel32 ntdll)
add_pch(winspool precomp.h "${PCH_SKIP_SOURCE}")
add_cd_file(TARGET winspool DESTINATION mergeos/system32 FOR all)
