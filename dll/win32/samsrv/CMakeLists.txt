
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl)

add_rpc_files(server ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/sam.idl)
spec2def(samsrv.dll samsrv.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    alias.c
    database.c
    display.c
    domain.c
    group.c
    registry.c
    samrpc.c
    samsrv.c
    security.c
    setup.c
    user.c
    utils.c
    samsrv.h
    ${CMAKE_CURRENT_BINARY_DIR}/sam_s.c
    ${CMAKE_CURRENT_BINARY_DIR}/samsrv_stubs.c)

add_library(samsrv MODULE
    ${SOURCE}
    samsrv.rc
    ${CMAKE_CURRENT_BINARY_DIR}/samsrv.def)

set_module_type(samsrv win32dll UNICODE)
target_link_libraries(samsrv wine ${PSEH_LIB})
add_importlibs(samsrv rpcrt4 advapi32 msvcrt kernel32 ntdll)
add_pch(samsrv samsrv.h SOURCE)
add_cd_file(TARGET samsrv DESTINATION mergeos/system32 FOR all)
