
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
add_definitions(-D__WINESRC__)
spec2def(mssip32.dll mssip32.spec)

list(APPEND SOURCE
    main.c
    ${CMAKE_CURRENT_BINARY_DIR}/mssip32_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/mssip32.def)

add_library(mssip32 MODULE ${SOURCE})
set_module_type(mssip32 win32dll)
target_link_libraries(mssip32 wine)
add_importlibs(mssip32 msvcrt kernel32 ntdll)
add_cd_file(TARGET mssip32 DESTINATION mergeos/system32 FOR all)
