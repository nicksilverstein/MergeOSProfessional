
include_directories(
    BEFORE include
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/drivers)

list(APPEND SOURCE
    afd/bind.c
    afd/connect.c
    afd/context.c
    afd/info.c
    afd/listen.c
    afd/lock.c
    afd/main.c
    afd/read.c
    afd/select.c
    afd/tdi.c
    afd/tdiconn.c
    afd/write.c
    include/afd.h)

add_library(afd MODULE ${SOURCE} afd.rc)
target_link_libraries(afd ${PSEH_LIB})
set_module_type(afd kernelmodedriver)
add_importlibs(afd ntoskrnl hal)
add_pch(afd include/afd.h SOURCE)
add_cd_file(TARGET afd DESTINATION mergeos/system32/drivers FOR all)
add_registry_inf(afd_reg.inf)
