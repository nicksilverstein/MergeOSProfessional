
add_definitions(-D__WINESRC__)
remove_definitions(-D_CRT_NON_CONFORMING_SWPRINTFS)
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/wine)

list(APPEND SOURCE
    msiexec.c
    service.c
    precomp.h)

add_rc_deps(rsrc.rc ${CMAKE_CURRENT_SOURCE_DIR}/msiexec.ico)
add_executable(msiexec ${SOURCE} rsrc.rc)
set_module_type(msiexec win32gui)
target_link_libraries(msiexec uuid wine)
add_importlibs(msiexec user32 advapi32 ole32 comctl32 msi msvcrt kernel32 ntdll)
add_pch(msiexec precomp.h SOURCE)
add_cd_file(TARGET msiexec DESTINATION mergeos/system32 FOR all)
