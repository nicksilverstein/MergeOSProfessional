
include_directories(
    ${MERGEOS_SOURCE_DIR}/sdk/lib/drivers/sound/soundblaster
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/sound)

add_library(audio time.c)
add_dependencies(audio bugcodes xdk)
