
include_directories(
    ${MERGEOS_SOURCE_DIR}/sdk/lib/drivers/sound/soundblaster
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/sound)

add_library(audioleg devname.c hardware.c)
add_dependencies(audioleg bugcodes xdk)
