
remove_definitions(-D_WIN32_WINNT=0x502)
add_definitions(-D_WIN32_WINNT=0x600)

add_definitions(-D__WINESRC__)
include_directories(BEFORE ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
spec2def(msctf.dll msctf.spec)

list(APPEND SOURCE
    categorymgr.c
    compartmentmgr.c
    context.c
    displayattributemgr.c
    documentmgr.c
    inputprocessor.c
    langbarmgr.c
    msctf.c
    range.c
    threadmgr.c
    precomp.h
    ${CMAKE_CURRENT_BINARY_DIR}/msctf_stubs.c)

add_library(msctf MODULE
    ${SOURCE}
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/msctf.def)

set_module_type(msctf win32dll)
target_link_libraries(msctf uuid wine)
add_importlibs(msctf ole32 oleaut32 user32 advapi32 advapi32_vista msvcrt kernel32 ntdll)
add_pch(msctf precomp.h SOURCE)
add_cd_file(TARGET msctf DESTINATION mergeos/system32 FOR all)
