
include_directories(BEFORE ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/sound)

list(APPEND SOURCE
    dsp_io.c
    version.c
    speaker.c
    rate.c
    mixer.c)

add_library(soundblaster ${SOURCE})
add_pch(soundblaster precomp.h)
add_dependencies(soundblaster bugcodes xdk)
