
add_library(buslogic MODULE BusLogic958.c BusLogic958.rc)
target_link_libraries(buslogic libcntpr)
set_module_type(buslogic kernelmodedriver)
add_importlibs(buslogic scsiport)
add_cd_file(TARGET buslogic DESTINATION mergeos/system32/drivers NO_CAB FOR all)
add_registry_inf(buslogic_reg.inf)
