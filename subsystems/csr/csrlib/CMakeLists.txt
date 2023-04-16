
add_definitions(-D_NTSYSTEM_)

include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/subsys/csr)

list(APPEND SOURCE
    api.c
    capture.c
    connect.c)

add_library(csrlib ${SOURCE})
add_pch(csrlib csrlib.h SOURCE)
add_dependencies(csrlib psdk)
