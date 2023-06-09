
include_directories(
    ${MERGEOS_SOURCE_DIR}/sdk/lib/evtlib
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/subsys
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl
    ${CMAKE_CURRENT_BINARY_DIR})

add_rpc_files(server ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/eventlogrpc.idl)

list(APPEND SOURCE
    eventlog.c
    eventsource.c
    logport.c
    rpc.c
    file.c
    eventlog.h
    ${CMAKE_CURRENT_BINARY_DIR}/eventlogrpc_s.c)

add_executable(eventlog ${SOURCE} eventlog.rc)
add_pch(eventlog eventlog.h SOURCE)

if(NOT MSVC)
    target_link_libraries(eventlog ${PSEH_LIB})
endif()

set_module_type(eventlog win32cui UNICODE)
target_link_libraries(eventlog evtlib)
add_importlibs(eventlog advapi32 rpcrt4 msvcrt kernel32 ntdll)
add_cd_file(TARGET eventlog DESTINATION mergeos/system32 FOR all)
