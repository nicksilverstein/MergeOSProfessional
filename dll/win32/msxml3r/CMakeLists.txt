
add_library(msxml3r MODULE msxml3r.rc)
add_dependencies(msxml3r psdk)
set_module_type(msxml3r win32dll ENTRYPOINT 0)
add_cd_file(TARGET msxml3r DESTINATION mergeos/system32 FOR all)
