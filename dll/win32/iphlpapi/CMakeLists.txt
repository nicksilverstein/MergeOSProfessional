
include_directories(
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine
    ${MERGEOS_SOURCE_DIR}/dll/win32/dhcpcsvc/include
    ${MERGEOS_SOURCE_DIR}/sdk/lib/tdilib)

spec2def(iphlpapi.dll iphlpapi.spec ADD_IMPORTLIB)

add_definitions(
    -DGetAdaptersAddressesV2
    -D_CRT_NO_POSIX_ERROR_CODES)

list(APPEND SOURCE
    address.c
    dhcp_mergeos.c
    icmp.c
    ifenum_mergeos.c
    ipstats_mergeos.c
    iphlpapi_mergeos.c
    iphlpapi_main.c
    media.c
    registry.c
    resinfo_mergeos.c
    route_mergeos.c
    iphlpapi_private.h
    ${CMAKE_CURRENT_BINARY_DIR}/iphlpapi_stubs.c)

add_library(iphlpapi MODULE
    ${SOURCE}
    iphlpapi.rc
    iphlpapi_fakestubs.c
    iphlpapi.spec
    ${CMAKE_CURRENT_BINARY_DIR}/iphlpapi.def)

set_module_type(iphlpapi win32dll UNICODE)
target_link_libraries(iphlpapi wine tdilib)
add_importlibs(iphlpapi dhcpcsvc advapi32 psapi ws2_32 msvcrt kernel32 ntdll)
add_pch(iphlpapi iphlpapi_private.h SOURCE)
add_cd_file(TARGET iphlpapi DESTINATION mergeos/system32 FOR all)
