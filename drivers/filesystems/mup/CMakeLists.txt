
list(APPEND SOURCE
     dfs.c
     mup.c
     dfs.h
     mup.h)

add_library(mup MODULE ${SOURCE} mup.rc)
set_module_type(mup kernelmodedriver)
target_link_libraries(mup ${PSEH_LIB})
add_importlibs(mup ntoskrnl hal)
add_pch(mup mup.h SOURCE)
add_cd_file(TARGET mup DESTINATION mergeos/system32/drivers FOR all)
