
include_directories(
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/subsys
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl
    ${CMAKE_CURRENT_BINARY_DIR})

add_rpc_files(server ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/svcctl.idl)

list(APPEND SOURCE
    config.c
    controlset.c
    database.c
    driver.c
    groupdb.c
    lock.c
    rpcserver.c
    security.c
    services.c
    services.h
    ${CMAKE_CURRENT_BINARY_DIR}/svcctl_s.c)

add_executable(services ${SOURCE} services.rc)
target_link_libraries(services ${PSEH_LIB})

set_module_type(services win32gui UNICODE)
add_importlibs(services userenv user32 advapi32 rpcrt4 msvcrt kernel32 ntdll)
add_pch(services services.h SOURCE)
add_cd_file(TARGET services DESTINATION mergeos/system32 FOR all)
