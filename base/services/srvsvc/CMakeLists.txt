
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl)
add_rpc_files(server ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/srvsvc.idl)
spec2def(srvsvc.dll srvsvc.spec ADD_IMPORTLIB)

add_library(srvsvc MODULE
    rpcserver.c
    srvsvc.c
    srvsvc.rc
    ${CMAKE_CURRENT_BINARY_DIR}/srvsvc_s.c
    ${CMAKE_CURRENT_BINARY_DIR}/srvsvc.def)

set_module_type(srvsvc win32dll UNICODE)
target_link_libraries(srvsvc wine)
add_importlibs(srvsvc advapi32 rpcrt4 msvcrt kernel32 ntdll)
add_cd_file(TARGET srvsvc DESTINATION mergeos/system32 FOR all)
