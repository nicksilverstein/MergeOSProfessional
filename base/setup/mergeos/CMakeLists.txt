
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
#    ${MERGEOS_SOURCE_DIR}/base/setup/lib
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/zlib
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/drivers)

list(APPEND SOURCE
    spapisup/fileqsup.c
    spapisup/infsupp.c
    drivepage.c
    mergeos.c
    treelist.c
    mergeos.h)

file(GLOB mergeos_rc_deps res/*.*)
add_rc_deps(mergeos.rc ${mergeos_rc_deps})
add_executable(mergeos ${SOURCE} mergeos.rc)
set_module_type(mergeos win32gui UNICODE)
add_pch(mergeos mergeos.h SOURCE)
target_link_libraries(mergeos uuid setuplib ext2lib vfatlib btrfslib)
add_importlibs(mergeos advapi32 gdi32 user32 comctl32 shlwapi setupapi msvcrt kernel32 ntdll)
add_cd_file(TARGET mergeos DESTINATION mergeos NO_CAB FOR bootcd)
