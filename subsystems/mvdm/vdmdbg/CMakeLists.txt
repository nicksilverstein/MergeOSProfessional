
spec2def(vdmdbg.dll vdmdbg.spec)

list(APPEND SOURCE
    vdmdbg.c
    ${CMAKE_CURRENT_BINARY_DIR}/vdmdbg.def)

add_library(vdmdbg MODULE ${SOURCE})
set_module_type(vdmdbg win32dll UNICODE)
add_importlibs(vdmdbg msvcrt kernel32 ntdll)
add_cd_file(TARGET vdmdbg DESTINATION mergeos/system32 FOR all)
