
spec2def(dxgthk.sys dxgthk.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    main.c
    dxgthk.rc
    ${CMAKE_CURRENT_BINARY_DIR}/dxgthk.def)

add_library(dxgthk MODULE ${SOURCE})
set_module_type(dxgthk kernelmodedriver)
add_importlibs(dxgthk win32k)
add_cd_file(TARGET dxgthk DESTINATION mergeos/system32/drivers FOR all)
