
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/subsys)

spec2def(basesrv.dll basesrv.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    appcompat.c
    dosdev.c
    init.c
    proc.c
    sndsntry.c
    vdm.c
    nls.c
    basesrv.h)

add_library(basesrv MODULE
    ${SOURCE}
    basesrv.rc
    ${CMAKE_CURRENT_BINARY_DIR}/basesrv.def)

set_module_type(basesrv win32dll UNICODE ENTRYPOINT DllMain 12)
#############################################
## HACK FOR MSVC COMPILATION WITH win32dll ##
set_subsystem(basesrv console)
################# END  HACK #################

target_link_libraries(basesrv ${PSEH_LIB})
add_importlibs(basesrv csrsrv ntdll)
add_pch(basesrv basesrv.h SOURCE)
add_dependencies(basesrv bugcodes xdk)
add_cd_file(TARGET basesrv DESTINATION mergeos/system32 FOR all)
