
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl)
add_rpc_files(server ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/atsvc.idl)
spec2def(schedsvc.dll schedsvc.spec ADD_IMPORTLIB)

add_library(schedsvc MODULE
    job.c
    rpcserver.c
    schedsvc.c
    schedsvc.rc
    ${CMAKE_CURRENT_BINARY_DIR}/atsvc_s.c
    ${CMAKE_CURRENT_BINARY_DIR}/schedsvc.def)

set_module_type(schedsvc win32dll UNICODE)
target_link_libraries(schedsvc wine)
add_importlibs(schedsvc advapi32 rpcrt4 msvcrt kernel32 ntdll)
add_cd_file(TARGET schedsvc DESTINATION mergeos/system32 FOR all)
