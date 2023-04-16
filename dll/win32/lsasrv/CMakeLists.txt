
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/subsys/lsass)

add_rpc_files(server
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/dssetup.idl
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/lsa.idl)
spec2def(lsasrv.dll lsasrv.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    authpackage.c
    authport.c
    database.c
    dssetup.c
    lookup.c
    lsarpc.c
    lsasrv.c
    notify.c
    policy.c
    privileges.c
    registry.c
    security.c
    service.c
    session.c
    srm.c
    utils.c
    lsasrv.h
    ${CMAKE_CURRENT_BINARY_DIR}/dssetup_s.c
    ${CMAKE_CURRENT_BINARY_DIR}/lsa_s.c
    ${CMAKE_CURRENT_BINARY_DIR}/lsasrv_stubs.c)

add_library(lsasrv MODULE
    ${SOURCE}
    lsasrv.rc
    ${CMAKE_CURRENT_BINARY_DIR}/lsasrv.def)

set_module_type(lsasrv win32dll UNICODE)
target_link_libraries(lsasrv wine ${PSEH_LIB})
add_importlibs(lsasrv samsrv rpcrt4 msvcrt kernel32 advapi32 ntdll)
add_pch(lsasrv lsasrv.h SOURCE)
add_cd_file(TARGET lsasrv DESTINATION mergeos/system32 FOR all)
