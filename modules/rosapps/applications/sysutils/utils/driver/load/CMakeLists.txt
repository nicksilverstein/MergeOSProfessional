
add_executable(load load.c)
set_module_type(load win32cui UNICODE)
add_importlibs(load ntdll user32 msvcrt kernel32)
add_cd_file(TARGET load DESTINATION mergeos/system32 FOR all)
