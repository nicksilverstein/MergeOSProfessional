
include_directories(${MERGEOS_SOURCE_DIR}/sdk/lib/conutils)

add_executable(netstat netstat.c netstat.rc)
set_module_type(netstat win32cui UNICODE)
target_link_libraries(netstat conutils ${PSEH_LIB})
add_importlibs(netstat user32 ws2_32 snmpapi iphlpapi msvcrt kernel32)
add_cd_file(TARGET netstat DESTINATION mergeos/system32 FOR all)
