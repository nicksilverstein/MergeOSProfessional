
list(APPEND SOURCE
	gdihv.c
	gdihv.rc
	mainwnd.c
	handlelist.c
	proclist.c)

add_executable(gdihv ${SOURCE})
set_module_type(gdihv win32cui UNICODE)
add_importlibs(gdihv user32 comctl32 psapi msvcrt kernel32)
add_cd_file(TARGET gdihv DESTINATION mergeos/system32 FOR all)
