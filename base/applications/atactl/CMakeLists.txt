
add_definitions(-DUSER_MODE)
include_directories(${MERGEOS_SOURCE_DIR}/drivers/storage/ide/uniata)
add_executable(atactl atactl.cpp atactl.rc)
set_module_type(atactl win32cui)
add_importlibs(atactl advapi32 msvcrt kernel32 ntdll)
add_cd_file(TARGET atactl DESTINATION mergeos/system32 FOR all)
