
PROJECT(msconfig_new)

include_directories(
    .
    comctl32ex
    ${MERGEOS_SOURCE_DIR}/sdk/include/c++)

list(APPEND C_SOURCE
    comctl32ex/comctl32supp.c
    comctl32ex/listview.c
    comctl32ex/treeview.c
    comctl32ex/uxthemesupp.c
    fileextractdialog.c
    fileutils.c
    freeldrpage.c
    generalpage.c
    msconfig.c
    regutils.c
    # startuppage.c
    stringutils.c
    utils.c)

list(APPEND CPP_SOURCE
    systempage.cpp
    srvpage.cpp
    toolspage.cpp
    xmldomparser.cpp
    precomp.h)

add_rc_deps(msconfig.rc ${CMAKE_CURRENT_SOURCE_DIR}/res/msconfig.ico)
add_executable(msconfig_new ${C_SOURCE} ${CPP_SOURCE} msconfig.rc)
add_pch(msconfig_new precomp.h CPP_SOURCE)
set_module_type(msconfig_new win32gui UNICODE)
target_link_libraries(msconfig_new comsupp cpprt atl_classes)
set_target_cpp_properties(msconfig_new WITH_EXCEPTIONS)
add_importlibs(msconfig_new user32 gdi32 comctl32 comdlg32 advapi32 version ole32 oleaut32 msxml3 shell32 shlwapi msvcrt kernel32)
add_cd_file(TARGET msconfig_new DESTINATION mergeos/system32 FOR all)
