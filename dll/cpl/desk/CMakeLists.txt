
add_definitions(-D_WIN32)
spec2def(desk.cpl desk.spec)

list(APPEND SOURCE
    advmon.c
    appearance.c
    background.c
    classinst.c
    desk.c
    desktop.c
    devsett.c
    dibitmap.c
    misc.c
    preview.c
    screensaver.c
    advappdlg.c
    effappdlg.c
    settings.c
    monslctl.c
    general.c
    draw.c
    theme.c
    muireg.c)

list(APPEND PCH_SKIP_SOURCE
    guid.c)

file(GLOB desk_rc_deps resources/*.*)
add_rc_deps(desk.rc ${desk_rc_deps})

add_library(desk MODULE
    ${SOURCE}
    ${PCH_SKIP_SOURCE}
    desk.rc
    ${CMAKE_CURRENT_BINARY_DIR}/desk.def)

set_module_type(desk cpl UNICODE)
target_link_libraries(desk uuid)
add_importlibs(desk user32 advapi32 gdi32 comctl32 comdlg32 ole32 setupapi shell32 shlwapi uxtheme gdiplus msvcrt kernel32 ntdll)
add_pch(desk desk.h "${PCH_SKIP_SOURCE}")
add_cd_file(TARGET desk DESTINATION mergeos/system32 FOR all)
