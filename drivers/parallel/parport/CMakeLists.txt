
list(APPEND SOURCE
    fdo.c
    misc.c
    pdo.c
    parport.c
    parport.h)

add_library(parport MODULE
    ${SOURCE}
    parport.rc)

set_module_type(parport kernelmodedriver)
add_pch(parport parport.h SOURCE)
add_importlibs(parport ntoskrnl hal)
add_cd_file(TARGET parport DESTINATION mergeos/system32/drivers FOR all)
