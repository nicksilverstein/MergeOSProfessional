
add_executable(autochk WIN32 autochk.c autochk.rc)
set_module_type(autochk nativecui)
target_link_libraries(autochk nt vfatlib vfatxlib ntfslib btrfslib ext2lib cdfslib)
add_importlibs(autochk ntdll)
add_cd_file(TARGET autochk DESTINATION mergeos/system32 FOR all)
