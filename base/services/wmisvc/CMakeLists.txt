spec2def(wmisvc.dll wmisvc.spec ADD_IMPORTLIB)

add_library(wmisvc MODULE
    wmisvc.c
    wmisvc.rc
    ${CMAKE_CURRENT_BINARY_DIR}/wmisvc.def)

set_module_type(wmisvc win32dll UNICODE)
add_importlibs(wmisvc advapi32 msvcrt kernel32 ntdll)
add_cd_file(TARGET wmisvc DESTINATION mergeos/system32/wbem FOR all)
