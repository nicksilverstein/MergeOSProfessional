
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/subsys)

list(APPEND SOURCE
    smclient.c
    precomp.h)

add_library(smlib ${SOURCE})
add_pch(smlib precomp.h SOURCE)
add_dependencies(smlib psdk)
