
spec2def(watchdog.sys watchdog.spec)

add_library(watchdog MODULE
    watchdog.c
    ${CMAKE_CURRENT_BINARY_DIR}/watchdog.def)

set_module_type(watchdog kernelmodedriver)
add_importlibs(watchdog ntoskrnl)
add_cd_file(TARGET watchdog DESTINATION mergeos/system32/drivers FOR all)
