
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
add_definitions(-D__WINESRC__)
spec2def(shfolder.dll shfolder.spec)

list(APPEND SOURCE
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/shfolder.def)

add_library(shfolder MODULE ${SOURCE})
set_module_type(shfolder win32dll ENTRYPOINT 0 )
target_link_libraries(shfolder wine)
add_importlibs(shfolder shell32 kernel32 ntdll)
add_dependencies(shfolder psdk)
add_cd_file(TARGET shfolder DESTINATION mergeos/system32 FOR all)
