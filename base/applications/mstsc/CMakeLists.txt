
list(APPEND SOURCE
    asn.c
    bitmap.c
    bsops.c
    cache.c
    channels.c
    connectdialog.c
    iso.c
    licence.c
    mcs.c
    mppc.c
    orders.c
    pstcache.c
    rdp5.c
    rdp.c
    secure.c
    settings.c
    ssl_calls.c
    tcp.c
    uimain.c
    win32.c
    bsops.h
    constants.h
    orders.h
    proto.h
    rdesktop.h
    resource.h
    types.h
    uimain.h
    precomp.h)

add_definitions(-DWITH_SSL)
file(GLOB mstsc_rc_deps res/*.*)
add_rc_deps(rdc.rc ${mstsc_rc_deps})
add_executable(mstsc ${SOURCE} rdc.rc)
set_module_type(mstsc win32gui UNICODE)
add_importlibs(mstsc user32 gdi32 comctl32 ws2_32 crypt32 secur32 advapi32 shell32 ole32 comdlg32 msvcrt kernel32)
add_pch(mstsc precomp.h SOURCE)
add_cd_file(TARGET mstsc DESTINATION mergeos/system32 FOR all)

if(MSVC)
    # Disable warning C4267: 'initializing': conversion from 'size_t' to 'uint16', possible loss of data
    target_compile_options(mstsc PRIVATE /wd4267)
endif()
