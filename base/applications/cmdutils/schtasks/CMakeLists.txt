
add_executable(schtasks schtasks.c)
target_link_libraries(schtasks wine)
set_module_type(schtasks win32cui UNICODE)
add_importlibs(schtasks oleaut32 ole32 msvcrt kernel32 ntdll)
add_cd_file(TARGET schtasks DESTINATION mergeos/system32 FOR all)
