
spec2def(hidparse.sys hidparse.spec ADD_IMPORTLIB)
add_definitions(-DDEBUG_MODE)

include_directories(
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/include
    ${MERGEOS_SOURCE_DIR}/sdk/lib/drivers/hidparser)


add_library(hidparse MODULE hidparse.c hidparse.rc ${CMAKE_CURRENT_BINARY_DIR}/hidparse.def)
set_module_type(hidparse kernelmodedriver)
add_importlibs(hidparse ntoskrnl)
target_link_libraries(hidparse hidparser_km)
add_cd_file(TARGET hidparse DESTINATION mergeos/system32/drivers NO_CAB FOR all)
