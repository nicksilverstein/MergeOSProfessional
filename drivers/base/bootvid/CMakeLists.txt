
spec2def(bootvid.dll bootvid.spec ADD_IMPORTLIB)

if((ARCH STREQUAL "i386") OR (ARCH STREQUAL "amd64"))
    if(SARCH STREQUAL "pc98")
        list(APPEND SOURCE
            i386/pc98/pc98.h
            i386/pc98/bootvid.c)
    elseif(SARCH STREQUAL "xbox")
        list(APPEND SOURCE
            i386/xbox/xbox.h
            i386/xbox/bootvid.c)
    else()
        list(APPEND SOURCE
            i386/pc/pc.h
            i386/pc/cmdcnst.h
            i386/pc/bootvid.c
            i386/pc/bootdata.c
            i386/pc/vga.h
            i386/pc/vga.c)
    endif()
elseif(ARCH STREQUAL "arm")
    list(APPEND SOURCE
        arm/arm.h
        arm/bootvid.c)
endif()

list(APPEND SOURCE
    common.c
    fontdata.c
    precomp.h)

add_library(bootvid MODULE
    ${SOURCE}
    bootvid.rc
    ${CMAKE_CURRENT_BINARY_DIR}/bootvid.def)

set_module_type(bootvid kerneldll ENTRYPOINT 0)
add_importlibs(bootvid ntoskrnl hal)
add_pch(bootvid precomp.h SOURCE)
add_dependencies(bootvid psdk)
add_cd_file(TARGET bootvid DESTINATION mergeos/system32 NO_CAB FOR all)
