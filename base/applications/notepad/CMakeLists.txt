
if(DBG)
    add_definitions(-D_DEBUG=1)
endif()

list(APPEND SOURCE
    dialog.c
    main.c
    printing.c
    settings.c
    text.c
    notepad.h)

add_rc_deps(rsrc.rc ${CMAKE_CURRENT_SOURCE_DIR}/res/notepad.ico)
add_executable(notepad ${SOURCE} rsrc.rc)
set_module_type(notepad win32gui UNICODE)
add_importlibs(notepad user32 gdi32 comctl32 comdlg32 advapi32 shell32 msvcrt kernel32)
add_pch(notepad notepad.h SOURCE)
add_cd_file(TARGET notepad DESTINATION mergeos FOR all)
add_cd_file(TARGET notepad DESTINATION mergeos/system32 FOR all)
