
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/drivers)

list(APPEND SOURCE
    blue.c
    font.c
    blue.h)

add_library(blue MODULE ${SOURCE} blue.rc)
set_module_type(blue kernelmodedriver)
add_importlibs(blue ntoskrnl hal)
add_pch(blue blue.h SOURCE)
add_cd_file(TARGET blue DESTINATION mergeos/system32/drivers NO_CAB FOR all)
