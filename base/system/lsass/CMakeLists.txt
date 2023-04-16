
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/subsys)
add_executable(lsass lsass.c lsass.rc)
set_module_type(lsass win32gui UNICODE)
add_importlibs(lsass advapi32 lsasrv samsrv msvcrt kernel32 ntdll)
add_cd_file(TARGET lsass DESTINATION mergeos/system32 FOR all)
