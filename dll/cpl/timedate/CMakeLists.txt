
include_directories(${MERGEOS_SOURCE_DIR}/sdk/lib/tzlib)

spec2def(timedate.cpl timedate.spec)

list(APPEND SOURCE
    clock.c
    dateandtime.c
    internettime.c
    monthcal.c
    timedate.c
    timezone.c
    timedate.h)

file(GLOB timedate_rc_deps resources/*.*)
add_rc_deps(timedate.rc ${timedate_rc_deps})

add_library(timedate MODULE
    ${SOURCE}
    timedate.rc
    ${CMAKE_CURRENT_BINARY_DIR}/timedate.def)

set_module_type(timedate cpl UNICODE)
target_link_libraries(timedate tzlib)
add_importlibs(timedate w32time advapi32 user32 gdi32 comctl32 ws2_32 iphlpapi msvcrt kernel32 ntdll)
add_pch(timedate timedate.h SOURCE)
add_cd_file(TARGET timedate DESTINATION mergeos/system32 FOR all)
