
list(APPEND SOURCE
    roothub.c
    usbuhci.c
    usbuhci.h)

add_library(usbuhci MODULE
    ${SOURCE}
    guid.c
    usbuhci.rc)

set_module_type(usbuhci kernelmodedriver)
add_importlibs(usbuhci usbport usbd hal ntoskrnl)
add_pch(usbuhci usbuhci.h SOURCE)
add_cd_file(TARGET usbuhci DESTINATION mergeos/system32/drivers NO_CAB FOR all)
