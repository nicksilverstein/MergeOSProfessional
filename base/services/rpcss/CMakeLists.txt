
add_rpc_files(server
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine/epm.idl
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine/irot.idl)


list(APPEND SOURCE
    epmp.c
    irotp.c
    rpcss_main.c
    setup.c
    precomp.h
    ${CMAKE_CURRENT_BINARY_DIR}/epm_s.c
    ${CMAKE_CURRENT_BINARY_DIR}/irot_s.c)

include_directories(${MERGEOS_BINARY_DIR}/sdk/include/mergeos/wine)
add_executable(rpcss ${SOURCE} rpcss.rc)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(rpcss PRIVATE "-Wno-cast-calling-convention")
endif()

target_link_libraries(rpcss ${PSEH_LIB} wine)
set_module_type(rpcss win32cui UNICODE)
add_importlibs(rpcss advapi32 rpcrt4 msvcrt kernel32 ntdll)
add_pch(rpcss precomp.h SOURCE)
add_cd_file(TARGET rpcss DESTINATION mergeos/system32 FOR all)
