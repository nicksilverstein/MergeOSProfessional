
add_executable(infinst infinst.c)
set_module_type(infinst win32cui)
add_importlibs(infinst ntdll user32 setupapi comdlg32 msvcrt kernel32)
add_cd_file(TARGET infinst DESTINATION mergeos/system32 FOR all)
