
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
add_executable(expand expand.c expand.rc)
set_module_type(expand win32cui)
add_importlibs(expand lz32 setupapi user32 msvcrt kernel32 ntdll)
add_cd_file(TARGET expand DESTINATION mergeos/system32 FOR all)
