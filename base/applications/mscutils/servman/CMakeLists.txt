
list(APPEND SOURCE
    control.c
    create.c
    delete.c
    dependencies_tv1.c
    dependencies_tv2.c
    export.c
    listview.c
    mainwnd.c
    misc.c
    progress.c
    propsheet.c
    propsheet_depends.c
    propsheet_general.c
    propsheet_logon.c
    propsheet_recovery.c
    query.c
    servman.c
    start.c
    stop.c
    stop_dependencies.c
    precomp.h)

file(GLOB servman_rc_deps res/*.*)
add_rc_deps(servman.rc ${servman_rc_deps})
add_executable(servman ${SOURCE} servman.rc)
set_module_type(servman win32gui UNICODE)
add_importlibs(servman user32 gdi32 advapi32 version comctl32 shell32 comdlg32 msvcrt kernel32 ntdll)
add_pch(servman precomp.h SOURCE)
add_cd_file(TARGET servman DESTINATION mergeos/system32 FOR all)
