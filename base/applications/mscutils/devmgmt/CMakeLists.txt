
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/dll/devmgr)

add_executable(devmgmt devmgmt.c devmgmt.rc)
set_module_type(devmgmt win32gui UNICODE)
add_importlibs(devmgmt devmgr msvcrt kernel32)
add_cd_file(TARGET devmgmt DESTINATION mergeos/system32 FOR all)
