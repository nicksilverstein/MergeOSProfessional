
spec2def(vfd.dll vfdlib.spec ADD_IMPORTLIB)

add_message_headers(ANSI vfdmsg_lib.mc)

list(APPEND SOURCE
    vfdctl.c
    vfdfat.c
    vfdguiopen.c
    vfdguisave.c
    vfdguitip.c
    vfdguiut.c
    vfdlib.c
    vfdshcfact.cpp
    vfdshext.cpp
    vfdshmenu.cpp
    vfdshprop.cpp
    vfdshutil.cpp
    vfdzip.c)

if(DBG)
    add_definitions(-D_DEBUG)
endif()

add_library(vfd MODULE
    ${SOURCE}
    vfdlib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/vfd.def)

include_directories(${MERGEOS_SOURCE_DIR}/modules/rosapps/include/vfd
                    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/zlib)
set_module_type(vfd win32dll ENTRYPOINT DllMain 12)
target_link_libraries(vfd zlib_solo uuid cppstl)
set_target_cpp_properties(vfd WITH_EXCEPTIONS)
add_importlibs(vfd advapi32 user32 gdi32 shell32 comdlg32 comctl32 ole32 version psapi msvcrt kernel32 ntdll)
add_dependencies(vfd vfdmsg_lib)
add_cd_file(TARGET vfd DESTINATION mergeos/system32 FOR all)
