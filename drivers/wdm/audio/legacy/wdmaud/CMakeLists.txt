
add_definitions(-D_COMDDK_)

include_directories(
    ${MERGEOS_SOURCE_DIR}/sdk/lib/drivers/sound/mmixer
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/sound)

list(APPEND SOURCE
    control.c
    deviface.c
    entry.c
    mmixer.c
    sup.c
    wdmaud.h)

add_library(wdmaud MODULE ${SOURCE} wdmaud.rc)
set_module_type(wdmaud kernelmodedriver)
target_link_libraries(wdmaud mmixer libcntpr)
add_pch(wdmaud wdmaud.h SOURCE)
add_importlibs(wdmaud ntoskrnl ks hal)
add_cd_file(TARGET wdmaud DESTINATION mergeos/system32/drivers FOR all)
