project(appcompat)
include_directories(${MERGEOS_SOURCE_DIR}/dll/appcompat/apphelp)
add_executable(sdbinst sdbinst.c)

set_module_type(sdbinst win32cui UNICODE)
add_importlibs(sdbinst shlwapi advapi32 apphelp ole32 msvcrt kernel32 ntdll)
add_cd_file(TARGET sdbinst DESTINATION mergeos/system32 FOR all)
