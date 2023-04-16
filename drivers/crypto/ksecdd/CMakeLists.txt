
spec2def(ksecdd.sys ksecdd.spec)

include_directories(
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/drivers/ksecdd
    ${MERGEOS_SOURCE_DIR}/sdk/lib/cryptlib)

list(APPEND SOURCE
    ksecdd.c
    dispatch.c
    crypt.c
    random.c
    stubs.c
    ksecdd.rc
    ${CMAKE_CURRENT_BINARY_DIR}/ksecdd.def)

add_library(ksecdd MODULE ${SOURCE})
target_link_libraries(ksecdd cryptlib pseh)
set_module_type(ksecdd kernelmodedriver)
add_importlibs(ksecdd ntoskrnl hal)
add_cd_file(TARGET ksecdd DESTINATION mergeos/system32/drivers NO_CAB FOR all)
add_registry_inf(ksecdd_reg.inf)
