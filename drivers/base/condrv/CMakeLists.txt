
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/drivers)

list(APPEND SOURCE
    condrv.c
    control.c
    dispatch.c
    condrv.h)

add_library(condrv MODULE
    ${SOURCE}
    condrv.rc)

set_module_type(condrv kernelmodedriver)
target_link_libraries(condrv ${PSEH_LIB})
add_importlibs(condrv ntoskrnl hal)
add_pch(condrv condrv.h SOURCE)
add_cd_file(TARGET condrv DESTINATION mergeos/system32/drivers NO_CAB FOR all)
