
add_library(iologmsg MODULE iologmsg.rc)
set_module_type(iologmsg module UNICODE)
add_dependencies(iologmsg ntiologc)
add_cd_file(TARGET iologmsg DESTINATION mergeos/system32 FOR all)
