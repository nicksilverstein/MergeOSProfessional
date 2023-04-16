
add_executable(ps ps.c)
set_module_type(ps win32cui)
add_importlibs(ps ntdll user32 msvcrt kernel32)
add_cd_file(TARGET ps DESTINATION mergeos/system32 FOR all)
