
file(GLOB pifmgr_rc_deps "res/*.*" "res_alt/*.*")
add_rc_deps(pifmgr.rc ${pifmgr_rc_deps})
add_library(pifmgr MODULE pifmgr.rc)
add_dependencies(pifmgr psdk)
set_module_type(pifmgr module UNICODE)
add_cd_file(TARGET pifmgr DESTINATION mergeos/system32 FOR all)
