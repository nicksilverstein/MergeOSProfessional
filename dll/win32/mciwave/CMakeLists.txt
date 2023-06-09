
add_definitions(
    -D__WINESRC__
    -D_WINE)

include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/wine)
spec2def(mciwave.dll mciwave.spec)

list(APPEND SOURCE
    mciwave.c
    ${CMAKE_CURRENT_BINARY_DIR}/mciwave.def)

add_library(mciwave MODULE ${SOURCE})
set_module_type(mciwave win32dll)
target_link_libraries(mciwave wine)
add_importlibs(mciwave user32 winmm msvcrt kernel32 ntdll)
add_cd_file(TARGET mciwave DESTINATION mergeos/system32 FOR all)

if(MSVC)
    # Disable warning C4305: '=': truncation from 'UINT' to 'WORD'
    target_compile_options(mciwave PRIVATE /wd4305)
else()
    target_compile_options(mciwave PRIVATE "-Wno-overflow")
endif()
