
list(APPEND SOURCE
    attrib.c
    blockdev.c
    btree.c
    cleanup.c
    close.c
    create.c
    devctl.c
    dirctl.c
    dispatch.c
    fastio.c
    fcb.c
    finfo.c
    fsctl.c
    mft.c
    misc.c
    ntfs.c
    rw.c
    volinfo.c
    ntfs.h)

add_library(ntfs MODULE ${SOURCE} ntfs.rc)
set_module_type(ntfs kernelmodedriver)
target_link_libraries(ntfs ${PSEH_LIB})
add_importlibs(ntfs ntoskrnl hal)
add_pch(ntfs ntfs.h SOURCE)
add_cd_file(TARGET ntfs DESTINATION mergeos/system32/drivers FOR all)
