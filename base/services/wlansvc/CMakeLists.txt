
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl)
add_rpc_files(server ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/idl/wlansvc.idl)

list(APPEND SOURCE
    wlansvc.c
    rpcserver.c
    precomp.h
    ${CMAKE_CURRENT_BINARY_DIR}/wlansvc_s.c)

add_executable(wlansvc ${SOURCE} wlansvc.rc)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(wlansvc PRIVATE "-Wno-cast-calling-convention")
endif()

if(NOT MSVC)
    target_link_libraries(wlansvc ${PSEH_LIB})
endif()

set_module_type(wlansvc win32cui UNICODE)
add_importlibs(wlansvc advapi32 rpcrt4 iphlpapi msvcrt kernel32 ntdll)
add_pch(wlansvc precomp.h SOURCE)
add_cd_file(TARGET wlansvc DESTINATION mergeos/system32 FOR all)
