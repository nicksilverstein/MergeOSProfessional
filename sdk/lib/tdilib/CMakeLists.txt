
# FIXFIX: You guys should move this header in include/mergeos/libs/iphlpapi!
include_directories(${MERGEOS_SOURCE_DIR}/dll/win32/iphlpapi)
list(APPEND SOURCE
    enum.c
    handle.c
    precomp.h)

add_library(tdilib ${SOURCE})
add_pch(tdilib precomp.h SOURCE)
add_dependencies(tdilib psdk)
