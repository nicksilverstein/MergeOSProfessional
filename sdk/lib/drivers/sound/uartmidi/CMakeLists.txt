
include_directories(
    ${MERGEOS_SOURCE_DIR}/sdk/lib/drivers/sound/soundblaster
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/sound)

add_library(uartmidi midiuart.c)
add_dependencies(uartmidi bugcodes xdk)
