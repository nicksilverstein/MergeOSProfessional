
include_directories(${MERGEOS_SOURCE_DIR}/sdk/lib/3rdparty/freetype/include)
spec2def(ftfd.dll ftfd.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    enable.c
    font.c
    glyph.c
    rosglue.c)

list(APPEND PCH_SKIP_SOURCE
    sprintf.c)

add_library(ftfd MODULE
    ${SOURCE}
    ${PCH_SKIP_SOURCE}
    freetype.rc
    ${CMAKE_CURRENT_BINARY_DIR}/ftfd.def)

set_module_type(ftfd kerneldll ENTRYPOINT FtfdEnableDriver 12)
target_link_libraries(ftfd freetype libcntpr)
add_pch(ftfd ftfd.h "${PCH_SKIP_SOURCE}")
add_importlibs(ftfd win32k)
add_dependencies(ftfd psdk)
add_cd_file(TARGET ftfd DESTINATION mergeos/system32 FOR all)
