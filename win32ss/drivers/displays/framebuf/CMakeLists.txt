
list(APPEND SOURCE
    enable.c
    palette.c
    pointer.c
    screen.c
    surface.c
    framebuf.h)

add_library(framebuf MODULE
    ${SOURCE}
    framebuf.rc)

set_module_type(framebuf kerneldll ENTRYPOINT DrvEnableDriver 12)
add_pch(framebuf framebuf.h SOURCE)
target_link_libraries(framebuf libcntpr)
add_importlibs(framebuf win32k)
add_dependencies(framebuf psdk)
add_cd_file(TARGET framebuf DESTINATION mergeos/system32 FOR all)
