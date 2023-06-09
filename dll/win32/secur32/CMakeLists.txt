
add_definitions(-D__SECUR32__)
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/subsys)
spec2def(secur32.dll secur32.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    dllmain.c
    lsalpc.c
    sspi.c
    stubs.c
    thunks.c
    wrapper.c
    wine/base64_codec.c
    wine/dispatcher.c
    wine/hmac_md5.c
    wine/kerberos.c
    wine/negotiate.c
    wine/ntlm.c
    wine/util.c
    precomp.h)

add_library(secur32 MODULE
    ${SOURCE}
    secur32.rc
    ${CMAKE_CURRENT_BINARY_DIR}/secur32_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/secur32.def)

set_module_type(secur32 win32dll UNICODE)
target_link_libraries(secur32 lsalib wine)
add_importlibs(secur32 advapi32 netapi32 msvcrt kernel32 ntdll)
add_pch(secur32 precomp.h SOURCE)
add_cd_file(TARGET secur32 DESTINATION mergeos/system32 FOR all)
