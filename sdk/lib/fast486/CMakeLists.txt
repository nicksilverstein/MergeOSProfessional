
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/fast486)

list(APPEND SOURCE
    debug.c
    fast486.c
    opcodes.c
    opgroups.c
    extraops.c
    common.c
    fpu.c)

add_library(fast486 ${SOURCE})
add_dependencies(fast486 xdk)
